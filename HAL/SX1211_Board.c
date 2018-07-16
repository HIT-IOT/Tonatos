/*
功能：SX1211芯片 SPI及IO 驱动程序 For MSP430
作者：张扬
编写日期：2016.12.25

更改日期：重写，增加DMA、SPI两种方法传输，一次只传一组数，不支持队列连续传输。
更改功能：2017.1.9
--------------------------------------------------------------------

综述：
该程序为 SX1211芯片   SPI及IO 配置程序，使用USCI_A0
该程序专为SX1211芯片定制。也可使用于SX1212。


特性、注意：
IO设置不合理会增大功耗！！该部分程序要求硬件设计匹配！！
SPI数据传输速率不可超过1M。
SPI配置传输速率不可超过4M。
本程序使用硬件SPI，可使用DMA0，以节省功耗
一次发送5个字节以上才能显出DMA的优势，否则用SPI直发能更快些。
注：本程序未考虑对看门狗的处理

该程序归上层(MAC)调用，无需手动调用。

只有SPI模式才能接收数据。
因为低功耗，大多数情况数据量较小，因此未考虑DMA模式大数据量接收问题。


由于SX1211要求每传一个字节CS必须升高，不可采用UCA0TXIFG为DMA的触发源！！！！！
因此采用TMRA2计时触发DMA！！！因此对时钟配置十分敏感！！！！！改动SMCLK/通信速率后必须重新配置发送时间！！！！重新配置触发时间！！！！！




void SX1211_Send_CFG_DMA (uint32 _srcAddress, uint16 _size);                        // SX1211发送命令，DMA模式
void SX1211_Send_Bytes_DMA (uint32 _srcAddress, uint16 _size);                      // SX1211发送字节，DMA模式
    DMA模式，仅用于发送，适合5个字节以上的连续发送。准备时间63us，1个字节11us


void SX1211_Send_CFG_SPI (uint32 _srcAddress, uint16 _size);                        // SX1211发送命令，SPI模式
void SX1211_Send_Bytes_SPI (uint32 _srcAddress, uint16 _size);                      // SX1211发送字节，SPI模式
    SPI模式，仅用于发送，适合5个字节以下的连续发送。准备时间16us，1个字节21us


void SX1211_Rec_CFG_SPI (uint32 _srcAddress, uint32 _recAddress, uint16 _size);     // SX1211接收命令，SPI模式
void SX1211_Rec_Bytes_SPI (uint32 _srcAddress, uint32 _recAddress, uint16 _size);   // SX1211接收字节，SPI模式
    SPI模式，用于发送/接收。准备时间16us，1个字节21us

*/

#include <bsp_dma.h>
#include <SX1211_Board.h>
#include <msp430.h>
#include <macros.h>
#include <spi.h>
#include <hal.h>

const digioConfig pinRadio_IRQ0 = {HAL_SX1211_IRQ0_PORT, HAL_SX1211_IRQ0_PIN, HAL_SX1211_IRQ0_PIN_BIT, HAL_DIGIO_INPUT,  0};        // IRQ0
const digioConfig pinRadio_IRQ1 = {HAL_SX1211_IRQ1_PORT, HAL_SX1211_IRQ1_PIN, HAL_SX1211_IRQ1_PIN_BIT, HAL_DIGIO_INPUT,  0};        // IRQ1

uint32 SendAddress, RecAddress;             // 发送和接收指针，用于SPI通信方式
uint16 Send_num, Rec_num;                   // 发送和接收长度，用于SPI通信方式

bool SX1211_SPI_BUSY = TRUE;                // SX1211 SPI 占用

void SX1211_SPI_Setup (void)                // 初始化SX1211通信
{
//  初始化SX1211   IO
    MCU_IO_INPUT (HAL_SX1211_DATA_PORT, HAL_SX1211_DATA_PIN, MCU_IO_PULLOFF);           // DATA输入
    MCU_IO_INPUT (HAL_SX1211_PLL_LOCK_PORT, HAL_SX1211_PLL_LOCK_PIN, MCU_IO_PULLOFF);   // PLL_LOCK输入
    MCU_IO_OUTPUT (HAL_SX1211_CS_CFG_PORT, HAL_SX1211_CS_CFG_PIN, 1);                   // NSS_CFG输出高

    MCU_IO_PERIPHERAL_1 (HAL_SX1211_CS_DATA_PORT, HAL_SX1211_CS_DATA_PIN, MCU_IO_DIR_OUTPUT);   // UCA0STE
    MCU_IO_PERIPHERAL_1 (HAL_SX1211_SPISCK_PORT, HAL_SX1211_SPISCK_PIN, MCU_IO_DIR_OUTPUT);     // UCA0CLK
    MCU_IO_PERIPHERAL_1 (HAL_SX1211_SPIMOSI_PORT, HAL_SX1211_SPIMOSI_PIN, MCU_IO_DIR_OUTPUT);   // UCA0SIMO
    MCU_IO_PERIPHERAL_1 (HAL_SX1211_SPIMISO_PORT, HAL_SX1211_SPIMISO_PIN, MCU_IO_DIR_INPUT);    // UCA0SOMI


//  初始化SPI, A0为主模式
    EUSCI_A_SPI_disable(HAL_SX1211_SPI_BASE);                               // 关闭SPI

    EUSCI_A_SPI_masterInit(HAL_SX1211_SPI_BASE,                             // UCA0
                           EUSCI_A_SPI_CLOCKSOURCE_SMCLK,                   // SMCLK
                           (uint32)HAL_CPU_CLOCK_KHz *1000,
                           1000000,                                         // 1M
                           EUSCI_A_SPI_MSB_FIRST,
                           EUSCI_A_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,
                           EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_LOW,
                           EUSCI_A_SPI_4PIN_UCxSTE_ACTIVE_LOW               // 4pin
                           );
    EUSCI_A_SPI_select4PinFunctionality(HAL_SX1211_SPI_BASE, EUSCI_A_SPI_ENABLE_SIGNAL_FOR_4WIRE_SLAVE);    // 使能CS引脚

//  初始化DMA，通道0
        /*
         * Configure DMA channel 0
         * Configure channel for SINGLE transfers
         * DMA interrupt flag will be set after every 0 transfers
         * Use UCA0TXIFG to trigger transfers
         * Transfer BYTE-to-BYTE
         * Trigger upon Rising Edge of Trigger Source Signal
         */
    DMA_initialize(DMA_CHANNEL_0,
             DMA_TRANSFER_SINGLE,
             0,
             DMA0TSEL__TA2CCR0,                                             // 见说明！！！！！
             DMA_SIZE_SRCBYTE_DSTBYTE,
             DMA_TRIGGER_RISINGEDGE);
        /*
         * DMA transfers inhibited during read-modify-write CPU operations
         */
    DMA_disableTransferDuringReadModifyWrite ();

    DMA_setDstAddress(DMA_CHANNEL_0, (uint32)&UCA0TXBUF, DMA_DIRECTION_UNCHANGED);  // 配置DMA0目标地址：UCA0TX，地址不变

    SX1211_SPI_BUSY = FALSE;                                                // 解除SPI占用
}


void SX1211_Send_CFG_DMA (uint32 _srcAddress, uint16 _size)             // SX1211发送命令，DMA模式
{
    if (_size == 0)     return;                                             // 无数据，退出

    EUSCI_A_SPI_disable(HAL_SX1211_SPI_BASE);                                       // 关闭SPI
    SX1211_SPI_BUSY = TRUE;                                                 // SPI占用
    MCU_IO_OUTPUT (HAL_SX1211_CS_DATA_PORT, HAL_SX1211_CS_DATA_PIN, 1);     // NSS_DAT输出高
    SX1211_CFG_CS();                                                        // CFG配置片选

//  开始发送流程                                                                                                          // 注：此处触发源可以是UCA0TXIFG，但为了减少能耗开支，不再更改
    DMA_setSrcAddress(DMA_CHANNEL_0, _srcAddress, DMA_DIRECTION_INCREMENT); // 配置DMA0原地址，地址递增
    DMA_setTransferSize(DMA_CHANNEL_0, _size);                              // 配置传输尺寸
    DMA_clearInterrupt(DMA_CHANNEL_0);                                      // 清中断标志位
    DMA_installInterrupt(DMA_CHANNEL_0, DMA0_isr);                           // 开启DMA0中断
    DMA_enableTransfers(DMA_CHANNEL_0);                                     // 使能DMA0

    TA2CTL = 0;                                                             // 关闭TA2计时
    TA2CCTL0 = 0;                                                           // TA2的CCR0用于DMA计时， 详见说明！！！
    EUSCI_A_SPI_clearInterruptFlag(HAL_SX1211_SPI_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);    // 清标志位
    EUSCI_A_SPI_enable(HAL_SX1211_SPI_BASE);                                // 开启SPI，开始传输
    EUSCI_A_SPI_setInterruptFlag(HAL_SX1211_SPI_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);  // 置标志位，重新触发

//  初始化Timer A1，配置时间为 SPI传输1个字节的时间 * 1.3                                                见说明！！！！！
    TA2CCR0 = 10;
    TA2CTL = TASSEL_2 + ID_2 + MC_1 + TACLR;                                // SMCLK/4, up mode, interrupt disabled
}


void SX1211_Send_Bytes_DMA (uint32 _srcAddress, uint16 _size)           // SX1211发送字节，DMA模式
{
    if (_size == 0)     return;                                             // 无数据，退出

    EUSCI_A_SPI_disable(HAL_SX1211_SPI_BASE);                               // 关闭SPI
    SX1211_SPI_BUSY = TRUE;                                                 // SPI占用
    SX1211_CFG_DE_CS ();                                                    // 非配置字，CS拉高


//  开始发送流程
    DMA_setSrcAddress(DMA_CHANNEL_0, _srcAddress, DMA_DIRECTION_INCREMENT); // 配置DMA0原地址，地址递增
    DMA_setTransferSize(DMA_CHANNEL_0, _size);                              // 配置传输尺寸
    DMA_clearInterrupt(DMA_CHANNEL_0);                                      // 清中断标志位
    DMA_installInterrupt(DMA_CHANNEL_0, DMA0_isr);                           // 开启DMA0中断
    DMA_enableTransfers(DMA_CHANNEL_0);                                     // 使能DMA0


    TA2CTL = 0;                                                             // 关闭TA2计时
    TA2CCTL0 = 0;                                                           // TA2的CCR0用于DMA计时， 详见说明！！！
    MCU_IO_PERIPHERAL_1 (HAL_SX1211_CS_DATA_PORT, HAL_SX1211_CS_DATA_PIN, MCU_IO_DIR_OUTPUT);   // CS使能
    EUSCI_A_SPI_clearInterruptFlag(HAL_SX1211_SPI_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);        // 清标志位
    EUSCI_A_SPI_enable(HAL_SX1211_SPI_BASE);                                // 开启SPI，开始传输
    EUSCI_A_SPI_setInterruptFlag(HAL_SX1211_SPI_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);          // 置标志位，重新触发

//  初始化Timer A1，配置时间为 SPI传输1个字节的时间 * 1.3                                                见说明！！！！！
    TA2CCR0 = 10;
    TA2CTL = TASSEL_2 + ID_2 + MC_1 + TACLR;                                // SMCLK/4, up mode, interrupt disabled
}


void SX1211_Send_CFG_SPI (uint32 _srcAddress, uint16 _size)                 // SX1211发送命令，SPI模式
{
    if (_size == 0)     return;                                             // 无数据，退出

    EUSCI_A_SPI_disable(HAL_SX1211_SPI_BASE);                               // 关闭SPI
    SX1211_SPI_BUSY = TRUE;                                                 // SPI占用
    MCU_IO_OUTPUT (HAL_SX1211_CS_DATA_PORT, HAL_SX1211_CS_DATA_PIN, 1);     // NSS_DAT输出高

    Send_num = _size -1;                                                    // 发送长度
    SendAddress = _srcAddress +1;                                           // 发送指针
    RecAddress = 0;                                                         // 接收指针归0

    SX1211_CFG_CS();                                                        // CFG配置片选
    EUSCI_A_SPI_enable(HAL_SX1211_SPI_BASE);                                // 开启SPI，开始传输
    EUSCI_A_SPI_clearInterruptFlag(HAL_SX1211_SPI_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT); // 清接收标志位
    EUSCI_A_SPI_enableInterrupt (HAL_SX1211_SPI_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);   // 开接收中断

    UCA0TXBUF = *(uint8 *)_srcAddress;                                  // 开始发送
}


void SX1211_Send_Bytes_SPI (uint32 _srcAddress, uint16 _size)               // SX1211发送字节，SPI模式
{
    if (_size == 0)     return;                                             // 无数据，退出

    EUSCI_A_SPI_disable(HAL_SX1211_SPI_BASE);                               // 关闭SPI
    SX1211_SPI_BUSY = TRUE;                                                 // SPI占用
    SX1211_CFG_DE_CS ();                                                    // 非配置字，CS拉高

    Send_num = _size -1;                                                    // 发送长度
    SendAddress = _srcAddress +1;                                           // 发送指针
    RecAddress = 0;                                                         // 接收指针归0

    MCU_IO_PERIPHERAL_1 (HAL_SX1211_CS_DATA_PORT, HAL_SX1211_CS_DATA_PIN, MCU_IO_DIR_OUTPUT);   // CS使能
    EUSCI_A_SPI_enable(HAL_SX1211_SPI_BASE);                                // 开启SPI，开始传输
    EUSCI_A_SPI_clearInterruptFlag(HAL_SX1211_SPI_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);         // 清接收标志位
    EUSCI_A_SPI_enableInterrupt (HAL_SX1211_SPI_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);           // 开接收中断

    UCA0TXBUF = *(uint8 *)_srcAddress;                                  // 开始发送
}


void SX1211_Read_CFG_SPI (uint32 _srcAddress, uint32 _recAddress, uint16 _size) // SX1211接收命令，SPI模式
{
    if (_size == 0)     return;                                             // 无数据，退出

    EUSCI_A_SPI_disable(HAL_SX1211_SPI_BASE);                               // 关闭SPI
    SX1211_SPI_BUSY = TRUE;                                                 // SPI占用
    MCU_IO_OUTPUT (HAL_SX1211_CS_DATA_PORT, HAL_SX1211_CS_DATA_PIN, 1);     // NSS_DAT输出高

    Send_num = _size -1;                                                    // 发送长度
    Rec_num = 0;                                                            // 接收长度
    SendAddress = _srcAddress +1;                                           // 发送指针
    RecAddress = _recAddress;                                               // 接收指针

    SX1211_CFG_CS();                                                        // CFG配置片选
    EUSCI_A_SPI_enable(HAL_SX1211_SPI_BASE);                                // 开启SPI，开始传输
    EUSCI_A_SPI_clearInterruptFlag(HAL_SX1211_SPI_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT); // 清接收标志位
    EUSCI_A_SPI_enableInterrupt (HAL_SX1211_SPI_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);   // 开接收中断

    UCA0TXBUF = *(uint8 *)_srcAddress;                                      // 开始发送
}


void SX1211_Read_Bytes_SPI (uint32 _srcAddress, uint32 _recAddress, uint16 _size)   // SX1211接收字节，SPI模式
{
    if (_size == 0)     return;                                             // 无数据，退出

    EUSCI_A_SPI_disable(HAL_SX1211_SPI_BASE);                               // 关闭SPI
    SX1211_SPI_BUSY = TRUE;                                                 // SPI占用
    SX1211_CFG_DE_CS ();                                                    // 非配置字，CS拉高

    Send_num = _size -1;                                                    // 发送长度
    Rec_num = 0;                                                            // 接收长度
    SendAddress = _srcAddress +1;                                           // 发送指针
    RecAddress = _recAddress;                                               // 接收指针

    MCU_IO_PERIPHERAL_1 (HAL_SX1211_CS_DATA_PORT, HAL_SX1211_CS_DATA_PIN, MCU_IO_DIR_OUTPUT);   // CS使能
    EUSCI_A_SPI_enable(HAL_SX1211_SPI_BASE);                                // 开启SPI，开始传输
    EUSCI_A_SPI_clearInterruptFlag(HAL_SX1211_SPI_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);     // 清接收标志位
    EUSCI_A_SPI_enableInterrupt (HAL_SX1211_SPI_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);       // 开接收中断

    UCA0TXBUF = *(uint8 *)_srcAddress;                                  // 开始发送
}



HAL_ISR_FUNCTION(SX1211_ISR,USCI_A0)                                    // UCSI A0 接收中断
{
    if (RecAddress == 0)                                                // 如仅为发送任务
    {
        Rec_num = UCA0RXBUF;                                            // 读取UCA0RXBUF，清中断标志
    }
    else                                                                // 如有接收任务
    {
        *(uint8 *)RecAddress = UCA0RXBUF;                               // 收到的数存入缓存
        RecAddress++;                                                   // 地址++
        Rec_num++;                                                      // 长度++
    }

    if (Send_num != 0)                                                  // 未发送完毕
    {
        UCA0TXBUF = *(uint8 *)SendAddress;                              // 发送下一个
        SendAddress++;                                                  // 地址++
        Send_num--;                                                     // 长度--
    }
    else                                                                // 发送完毕
    {
        SX1211_CFG_DE_CS ();                                            // 配置CS拉高
        EUSCI_A_SPI_disableInterrupt (HAL_SX1211_SPI_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);  // 关接收中断
        EUSCI_A_SPI_disable(HAL_SX1211_SPI_BASE);                       // 关闭SPI
        SX1211_SPI_BUSY = FALSE;                                        // 解除SPI占用
        __low_power_mode_off_on_exit();                                 // 退出SPI并唤醒CPU
    }
}


void DMA0_isr (void)                                                    // DMA0中断处理程序
{   //  至此前一阶段发送完成
    SX1211_CFG_DE_CS ();                                                // 配置CS拉高
    DMA_disableInterrupt(DMA_CHANNEL_0);                                // 关闭DMA0中断
    TA2CTL = 0;

    SX1211_SPI_BUSY = FALSE;                                            // 解除SPI占用
    DMA_WAKE_INT = TRUE;                                                // 退出DMA并唤醒CPU
}

