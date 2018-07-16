/*  诊断器 2.3版。2017.6.12
    目标：在尽可能不影响功耗的前提下，实现数据的收发
    本程序仅实现文本（ASCII）的输出，不适合数据的大量收发
        如需对数据操作，需要修改代码
    本程序暂未考虑数据校验，有传输错误的可能
    本诊断器不检测外围模块状态，只收发数据
    本程序已考虑嵌套问题
    本程序仅支持英文字符串显示，不支持图形与中文！
    本程序使用缓存+DMA2+硬件SPI发送数据
    本程序缓存非环结构，存满后必须等待全部发送完毕方可再次写入。
    由于元件和线路限制，传输速率不可高于500K！！！

    定义 0x10-0x1F 为指令码。
    接收端接收到不识别的操作码，会清空接收缓存并复位
    其余为字符。接收端不处理不识别的字符（>=0x80）。

    本程序支持四种指令：输出字符、定位光标、清屏、LED管
        对应指令码：0x11、0x12、0x13、0x14
    支持的通配符：%d:有符号int        %u:无符号int       %x:16位16进制整数        %l:32位有符号整数     %%
                \b  \n

    DMA须以MCLK为时钟源，因此SPI也以SPI为时钟源，加快速度

    本程序仍有BUG，在发送大量小数据包时可能传输错误。
*/
/***********************************************************************************
* INCLUDES
*/

#include <diagnose.h>
#include <macros.h>
#include <stdarg.h>
#include <types.h>
#include <msp430.h>
#include <hal.h>
#include <dma.h>
#include <spi.h>

static bool diag_flag = TRUE;                                   // 诊断模块标志 (初始时占用，调用init_diag后才使能本模块)
static bool diag_sending = FALSE;                               // 初始未发送

uint8   Diag_BUF[HAL_BOARD_DIAG_BUF_LENGTH];                        // 发送数据缓冲区

static uint16   Diag_ptr_head, Diag_ptr_tail;                       // 缓冲区首尾指针


void init_diag (void)                                               // 诊断初始化（需在晶振初始化后调用！！）
{
//  初始化SPI A1为主模式

    MCU_IO_PERIPHERAL_1 (HAL_BOARD_DIAG_SCK_PORT, HAL_BOARD_DIAG_SCK_PIN, MCU_IO_DIR_OUTPUT);   // UCA1 IO
    MCU_IO_PERIPHERAL_1 (HAL_BOARD_DIAG_STE_PORT, HAL_BOARD_DIAG_STE_PIN, MCU_IO_DIR_OUTPUT);   // UCA1 IO
    MCU_IO_PERIPHERAL_1 (HAL_BOARD_DIAG_MOSI_PORT, HAL_BOARD_DIAG_MOSI_PIN, MCU_IO_DIR_OUTPUT); // UCA1 IO
    MCU_IO_PERIPHERAL_1 (HAL_BOARD_DIAG_MISO_PORT, HAL_BOARD_DIAG_MISO_PIN, MCU_IO_DIR_OUTPUT); // UCA1 IO

    EUSCI_A_SPI_disable(HAL_BOARD_DIAG_BASE);                               // 关闭SPI

    EUSCI_A_SPI_masterInit(HAL_BOARD_DIAG_BASE,                             // UCA1
                           EUSCI_A_SPI_CLOCKSOURCE_SMCLK,                   // SMCLK
                           (uint32)HAL_CPU_CLOCK_KHz *1000,
                           250000,                                          // 250K
                           EUSCI_A_SPI_MSB_FIRST,
                           EUSCI_A_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,
                           EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_LOW,
                           EUSCI_A_SPI_4PIN_UCxSTE_ACTIVE_HIGH              // 4线，CS高有效
                           );

    EUSCI_A_SPI_select4PinFunctionality(HAL_BOARD_DIAG_BASE, EUSCI_A_SPI_ENABLE_SIGNAL_FOR_4WIRE_SLAVE);    // 使能CS引脚

//  初始化DMA，通道2
        /*
         * Configure DMA channel 2
         * Configure channel for SINGLE transfers
         * DMA interrupt flag will be set after every 0 transfers
         * Use UCA1TXIFG to trigger transfers
         * Transfer BYTE-to-BYTE
         * Trigger upon Rising Edge of Trigger Source Signal
         */
    DMA_initialize(DMA_CHANNEL_2,
             DMA_TRANSFER_SINGLE,
             0,
             DMA2TSEL__UCA1TXIFG,
             DMA_SIZE_SRCBYTE_DSTBYTE,
             DMA_TRIGGER_RISINGEDGE);
        /*
         * DMA transfers inhibited during read-modify-write CPU operations
         */
    DMA_disableTransferDuringReadModifyWrite ();

        /*
         * Configure DMA channel 2
         * Use UCA1TXBUF as destination
         * destination address UNCHANGED
         */
    DMA_setDstAddress(DMA_CHANNEL_2,
                      (uint32)&UCA1TXBUF,
                      DMA_DIRECTION_UNCHANGED);


//  初始化变量
    Diag_ptr_head = Diag_ptr_tail = 0;                                      // 首尾地址初值为0
    diag_flag = FALSE;                                                      // 程序未被调用
    diag_sending = FALSE;                                                   // 初始未发送
}



#pragma __printf_args
void diagnose (const uint8 *ptr,...)                            // 发送诊断信息
{
    if (diag_flag == TRUE)  return;                                         // 程序已被调用，防止嵌套，退出
    if (Diag_ptr_tail >= HAL_BOARD_DIAG_BUF_LENGTH)     return;             // 缓存满，退出
    diag_flag = TRUE;                                                       // 置标志位


    Diag_BUF[Diag_ptr_tail] = 0x11;                                         // 指令：字符
    Diag_ptr_tail++;

//  %d      %u      %x      %%
    va_list app;
    va_start(app, ptr);                                                  // 变长传参

    while (*ptr != '\0')
    {
        if (Diag_ptr_tail >=HAL_BOARD_DIAG_BUF_LENGTH)  break;              // 缓存满，跳出

        Diag_BUF[Diag_ptr_tail] = *ptr;                                     // 直接输出
        Diag_ptr_tail++;

                    if (*ptr == '%')                                        // 如为%
                    {
                        ptr++;
                        Diag_BUF[Diag_ptr_tail] = *ptr;                     // 直接输出通配符
                        Diag_ptr_tail++;

                    /***********************    有符号int  ************************************************************/
                        if (*ptr == 'd')                                    // 检测类型，有符号十进制整数(int)
                        {
                            uint16 app_value;
                            app_value = va_arg (app, uint16);               // 读参

                            Diag_BUF[Diag_ptr_tail] = HI_UINT16 (app_value);// 高8位
                            Diag_ptr_tail++;
                            Diag_BUF[Diag_ptr_tail] = LO_UINT16 (app_value);// 低8位
                            Diag_ptr_tail++;
                        }

                    /***********************    无符号int  ************************************************************/
                        else if (*ptr == 'u')                                   // 检测类型，无符号十进制整数(unsigned int)
                        {
                            uint16 app_value;
                            app_value = va_arg (app, uint16);               // 读参

                            Diag_BUF[Diag_ptr_tail] = HI_UINT16 (app_value);// 高8位
                            Diag_ptr_tail++;
                            Diag_BUF[Diag_ptr_tail] = LO_UINT16 (app_value);// 低8位
                            Diag_ptr_tail++;
                        }

                    /***********************    16位16进制整数   ************************************************************/
                        else if (*ptr == 'x')                                   // 检测类型，使用无符号十六进制整数
                        {
                            uint16 app_value;
                            app_value = va_arg (app, uint16);               // 读参

                            Diag_BUF[Diag_ptr_tail] = HI_UINT16 (app_value);// 高8位
                            Diag_ptr_tail++;
                            Diag_BUF[Diag_ptr_tail] = LO_UINT16 (app_value);// 低8位
                            Diag_ptr_tail++;
                        }

                    /***********************    32位有符号整数    ************************************************************/
                        else if (*ptr == 'l')                                   // 检测类型，有符号十进制整数(long)
                        {
                            uint32 app_value;
                            app_value = va_arg (app, uint32);               // 读参

                            Diag_BUF[Diag_ptr_tail] = BREAK_UINT32 (app_value, 3);// 24-31位
                            Diag_ptr_tail++;
                            Diag_BUF[Diag_ptr_tail] = BREAK_UINT32 (app_value, 2);// 16-23位
                            Diag_ptr_tail++;
                            Diag_BUF[Diag_ptr_tail] = BREAK_UINT32 (app_value, 1);// 8-15位
                            Diag_ptr_tail++;
                            Diag_BUF[Diag_ptr_tail] = BREAK_UINT32 (app_value, 0);// 0-7位
                            Diag_ptr_tail++;
                        }
                    }

        ptr++;
    }

    diag_sendout ();                                                        // 数据发送

    diag_flag = FALSE;                                                      // 程序解除调用
}


void diag_cur (uint8 cur_y, uint8 cur_x)                        // 显示坐标设定
{
    if (diag_flag == TRUE)  return;                                         // 程序已被调用，防止嵌套，退出
    if (Diag_ptr_tail >= (HAL_BOARD_DIAG_BUF_LENGTH -3))    return;         // 检测是否有空间
    diag_flag = TRUE;                                                       // 置标志位

    Diag_BUF[Diag_ptr_tail] = 0x12;                                         // 指令：坐标
    Diag_ptr_tail++;
    Diag_BUF[Diag_ptr_tail] = cur_y;                                        // Y 坐标
    Diag_ptr_tail++;
    Diag_BUF[Diag_ptr_tail] = cur_x;                                        // X 坐标
    Diag_ptr_tail++;

    diag_sendout ();                                                        // 数据发送
    diag_flag = FALSE;                                                      // 程序解除调用
}


void diag_clrscr (void)                                         // 诊断清屏
{
    if (diag_flag == TRUE)  return;                                         // 程序已被调用，防止嵌套，退出
    if (Diag_ptr_tail >= (HAL_BOARD_DIAG_BUF_LENGTH -1))    return;         // 检测是否有空间
    diag_flag = TRUE;                                                       // 置标志位

    Diag_BUF[Diag_ptr_tail] = 0x13;                                         // 指令：清屏
    Diag_ptr_tail++;

    diag_sendout ();                                                        // 数据发送
    diag_flag = FALSE;                                                      // 程序解除调用
}


void diag_power (uint16 _level)                                 // 电量显示
{
    if (diag_flag == TRUE)  return;                                         // 程序已被调用，防止嵌套，退出
    if (Diag_ptr_tail >= (HAL_BOARD_DIAG_BUF_LENGTH -3))    return;         // 检测是否有空间
    diag_flag = TRUE;                                                       // 置标志位

    _level &= 0x7FFF;                                                       // 最高位为LCD的RST，不可为1

    Diag_BUF[Diag_ptr_tail] = 0x14;                                         // 指令：电量
    Diag_ptr_tail++;
    Diag_BUF[Diag_ptr_tail] = HI_UINT16 (_level);                           // 高8位
    Diag_ptr_tail++;
    Diag_BUF[Diag_ptr_tail] = LO_UINT16 (_level);                           // 低8位
    Diag_ptr_tail++;

    diag_sendout ();                                                        // 数据发送
    diag_flag = FALSE;                                                      // 程序解除调用
}


uint8 diag_busy (void)                                          // 检测DIAG是否忙        本函数未调试完
{
    if (diag_sending == TRUE)       return (TRUE);
    else    return (FALSE);
}


void diag_sendout (void)                                        // 发送缓存数据
{
    if (diag_sending == TRUE)   return;                                     // 如正在发送，返回

    while (EUSCI_A_SPI_isBusy (HAL_BOARD_DIAG_BASE) == EUSCI_A_SPI_BUSY);   // 等待之前最后一字节发送完成


    EUSCI_A_SPI_disable(HAL_BOARD_DIAG_BASE);                               // 关闭SPI
    EUSCI_A_SPI_clearInterruptFlag(HAL_BOARD_DIAG_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);    // 清标志位

//  开始发送流程
    DMA_setSrcAddress(DMA_CHANNEL_2, ((uint32)&Diag_BUF) + Diag_ptr_head, DMA_DIRECTION_INCREMENT);    // 配置DMA2原地址
    DMA_setTransferSize(DMA_CHANNEL_2, (Diag_ptr_tail - Diag_ptr_head));    // 配置传输尺寸
    DMA_clearInterrupt(DMA_CHANNEL_2);                                      // 清中断标志位
    DMA_installInterrupt(DMA_CHANNEL_2, DMA2_diag_isr);                      // 开启DMA2中断
    DMA_enableTransfers(DMA_CHANNEL_2);                                     // 使能DMA2

    Diag_ptr_head = Diag_ptr_tail;                                          // 数据已处理，首指针后移

    diag_sending = TRUE;                                                    // 发送中标志        必须置在开启SPI之前！！
    EUSCI_A_SPI_enable(HAL_BOARD_DIAG_BASE);                                // 开启SPI，开始传输
}


void DMA2_diag_isr (void)                                       // DMA发送中断处理程序
{
//  阶段性发送已经完成，检查是否还有数据待发送

    if (Diag_ptr_head == Diag_ptr_tail)                                     // 如数据全部发送完毕
    {
        DMA_disableInterrupt(DMA_CHANNEL_2);                                // 关闭DMA2中断
        Diag_ptr_head = Diag_ptr_tail = 0;                                  // 首尾地址归0
        diag_sending = FALSE;                                               // 清发送标志
    }
    else
    {
        DMA_setSrcAddress(DMA_CHANNEL_2, ((uint32)&Diag_BUF) + Diag_ptr_head, DMA_DIRECTION_INCREMENT);     // 配置DMA2原地址
        DMA_setTransferSize(DMA_CHANNEL_2, (Diag_ptr_tail - Diag_ptr_head));    // 配置传输尺寸
        Diag_ptr_head = Diag_ptr_tail;                                          // 数据已处理，首指针后移

        while ((EUSCI_A_SPI_getInterruptStatus(HAL_BOARD_DIAG_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT)) ==0);  // 为保险起见，等待上一次发送完毕，发送缓存空出，然后重新触发（实际上等待时间为0）

        EUSCI_A_SPI_clearInterruptFlag(HAL_BOARD_DIAG_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);// 清标志位
        DMA_enableTransfers(DMA_CHANNEL_2);                                                 // 使能DMA2
        EUSCI_A_SPI_setInterruptFlag(HAL_BOARD_DIAG_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);  // 置标志位，重新触发
    }
}
