
/*
功能：SX1211 无线芯片 物理层驱动模块 for MSP430
作者：张扬
完成日期：2017.1.9

更改日期：2017.9.22
更改功能：完善文档，挑BUG

--------------------------------------------------------------------

综述：
本程序为SX1211 无线芯片 物理层驱动模块 for MSP430。本程序仅提供硬件接口。
本程序也可使用于SX1212。

SX1211适用于863 - 960MHz，SX1212适用于300-510MHz

SX1211最大特点：接收电流3mA

SX1211参数：
    五种工作模式：休眠、待机、就绪、接收、发送
    三种数据模式：IO、字节、数据包
            IO模式：时钟+数据两线SPI
            字节模式：数据按字节传送，使用缓存
            数据包模式：使用完整的数据包发送数据，包含：前导、同步字、地址、长度、载荷、CRC。数据包自动生成
    3mA的接收电流，最大发射电流25mA，最大通信速率200Kb/s
    调制模式为FSK或OOK，发送功率-8dBm - +13dBm，工作电压2.1-3.6V
    内置64字节的FIFO，只能处于发送/接收状态，收到数据包后必须读取缓存方可接收后续数据包



注意：
本模块未考虑载波监听/空闲发送，发送时可能导致总线冲突。可在RX模式下通过SX1211_GET_RSSI ()获得当前信号强度，并判断总线是否占用。
强烈推荐通过软件生成初始化参数，参数存于SX1211_PreSetCfg[][]中。
本驱动程序的频率特性不可更改，需要通过软件生成，并更新配置数组。(如通信频率、速率等)
为了降低功耗，减小CPU时间，本程序采用单任务等待方式。

本驱动程序主要侧重于FSK模式通信，对OOK仅有基本的支持，有些参数不可更改。
本程序调用的SX1211_Board.c对SMCLK有要求，见说明
本程序初始化使用了TA2
本模块很多子程序仅用于特定的环境（如仅用于数据包模式，仅用于OOK等），但程序中未检测使用环境是否适合，需自行参照说明书注意
数据收/发时进行SPI通信可能会影响灵敏度。

载波频率及通信特性取决于无线模块的硬件设计，使用前应先测试模块的频率特性，选择合适的通信参数。
    天线应匹配模块的频率特性。（无线模块挑天线）
信号的质量与天线的类型、位置、摆放有很大关系，通信异常时可尝试重新摆放天线。
    经实测，人体及周围环境对信号质量没有本质上的影响。
    焊接时严格控制温度时间，切勿长时间加热无线模块。
工作电压不得超过3.6V，使用时需严格确保供电平稳，否则极可能出现各种异常！！！！！
SX1211可能会死锁，初始化无效，此时应断电重启

可变长度数据包模式时，发送数据包的第一个字节必须为长度字节，且不可为0。                                                                        ！！！
当接收到多个数据而MCU未响应时，会导致RX FIFO溢出，不再产生接收中断。                                                                     ！！！
    当MCU状态不确定时，需检测CC芯片状态(RF_Mode_Status)，确保其处于正常接收状态。                                                           ！！！
    当芯片状态异常时，可调用RF_Mode_RX 重新进入接收模式。                                                                                    ！！！

数据包模式下，某些同步字（如0xAAAA）会导致通信失效，需要更换其它值尝试                                                                      ！！！
数据包模式，PktParam1中的值为RX时最大接收的数据长度，设置过小会导致无法接收！！
未考虑多帧连发，发送数据应控制在64字节以内
如开启CRC，校验失败的包直接扔掉不处理
接收FIFO数据时，应收到EMPTY为止
    SX1211_FIFO_Standby (SX1211_STDBY_FIFO_READ);           // 使得接收到数据后，进入STDBY模式也可收数
调用 SX1211_Board 中的函数前，一定确保 SX1211_SPI_BUSY 为假，否则原有通信数据直接被打断并取消
本模块暂未考虑看门狗



SX1211/SX1212通信说明：
    收发必须使用相同芯片、频段。
工作模式：
    主要有睡眠、待机、就绪、发送、接收五种模式。
    睡眠：芯片停止工作，最低电流3uA，唤醒需要时间。
    待机：芯片就绪，可对寄存器/缓存操作。平均电流1.7mA。
    就绪：VCO就绪，随时准备收发。平均电流1.7mA。
    发送：将FIFO中的数据发送出去。瞬时耗电约17mA/1dbm。
    接收：进入接收状态。持续耗电约17mA。

调制模式：
    本模块支持两种调制模式，FSK/OOK，通过SX1211_Modulation_Select()选择

地址：
    可以无地址校验，相同频段通道即可通信；加入地址校验后，只有相同地址或广播数据才可接收。具体参考 SX1211_Address_FILT()   SX1211_Address()
    注：如果地址匹配失败，则丢弃该数据包，并重新启动接收模式。                                                                               ！！！

长度：
    支持两种数据长度，通过设置 SX1211_Length_Set()   SX1211_Length()可选择数据长度
        固定数据包长度模式。数据包长度由 PktParam1 寄存器来设置。过长的数据包将不予接收。                                                  ！！！
        可变数据包长度模式。通过同步字后面的第一个字节来配置数据包长度，但不包括长度字节自身。
                PktParam1 寄存器用于设置RX模式中允许的最大数据包长度。任何长度字节值大于 PktParam1 的接收数据包将被丢弃。
    本模块设定为可变数据包长度模式。数据长度须小于等于64(含长度字节)。
    本发送模块将待发送数据一次写入缓存后统一发送。
    本模块暂未考虑IO模式和字节模式。



工作状态图：
                                                            ---------
                                                            |       |
                                                            |   TX  |
                                                            |       |
                                                            ---------
                                                              ^   |
                                                              |   |
                                                              |   v
    ---------                   ---------     ---------     ---------
    |       | initiate_SX1211();|       |---->|       |---->|       |
    | INIT  |------------------>| SLEEP |     | STDBY |     |   FS  |
    |       |                   |       |<----|       |<----|       |
    ---------                   ---------     ---------     ---------
                                                              |   ^
                                                              |   |
                                                              v   |
                                                            ---------                       -------------
                                                            |       |                       |           |
                                                            |   RX  |---------------------->|  Receive  |
                                                            |       |                       |           |
                                                            ---------                       -------------
实际上五种状态可以随意切换，但实际过程如上图所示。




细节说明：

CLKOUT在上电后自动开启，检测到CLKOUT说明芯片存在并已就绪。
    该信号功耗比430自带的晶振大，可关闭以节能。从休眠中唤醒时该信号保持其休眠前的状态。
    本程序为检测芯片是否就绪，该信号应一直开启。

IF2推荐为100KHz
FSK时，Fdev为采样频率，Fdev至少33K
OOK时，Fdev建议100K
具体配置见表37、38

RSSI 误差±3db，每dev周期测16次，滚筒取均值，因此其值每dev周期更新16次


本驱动未考虑的功能有：
    连续数据发送
    IO模式
    字节模式
    ASK模式

成功调试环境：MCU主频4M，MSP430FR6979，IAR EW430 6300
本模块一次只收发一个包。半双工通信



接口、函数说明：
    d为上电默认值，非本模块初始化默认值

uint8 initiate_SX1211 (void)：初始化模块。
    本子程序初始化SPI，初始化芯片内部寄存器，配置工作模式。
    具体配置信息参照配置寄存器SX1211_PreSetCfg[][] 及其说明。强烈推荐通过软件生成初始化参数



PLL锁定时间约300uS

缓存清空


*/

#include <SX1211.h>

#include <hal.h>
#include <timer_a.h>
#include <macros.h>
#include <MAC_radio.h>

/*
射频特性：           详见表37
    第一频段：               868M
    第二频段：               869M
    采样速率：               100K        默认
    位速率：                25K         默认
    实际传输速率：         3KB/s
    实际数据包传输速率：  330 - 2.7K Byte/s
    前导字：                25C4 H

预置参数：
    工作模式：               休眠
    数据模式：               数据包
    调制模式：               FSK
    FIFO尺寸：             64字节
    发送功率：               -8dBm
    前导/同步字：         3字节/2字节
    地址校验：               关闭
    数据长度：               变长
    CRC校验：              开启
    CRC自动清除：            开启
    曼彻斯特编码：         关闭
    数据白化：               关闭
    CLKOUT输出：           开启，200K
    PLL_LOCK输出：         开启
    同步字：                25C4 H
    IRQ0：                   接到包
    IRQ1：                   接收CRC_OK或发送完成
    发送：                 超出FIFO阈值发送
    待机模式FIFO：           写入


默认配置：
    字节0：    McParam1        休眠模式，868M范围，VCO默认，第一组频率
    字节1：    McParam2        FSK，数据包模式，OOK阈值，放大增益0dB
    字节2：    Fdev            采样率，/软件生成           100K        详见表37，不可低于33K
    字节3：    Bitrate         数据传输率，/软件生成     25K         详见表37
    字节4：    OokFloorThresh  OOK接收阈值下限
    字节5：    McParam6        FIFO，FIFO阈值             64字节/1字节
    字节6：    R1              R1，/软件生成
    字节7：    P1              P1，/软件生成
    字节8：    S1              S1，/软件生成                868M
    字节9：    R2              R2，/软件生成
    字节A：    P2              P2，/软件生成
    字节B：    S2              S2，/软件生成                869M
    字节C：    PaRamp          OOK电平时间                 23uS

    字节D：    IrqParam1       IRQ0收到包，IRQ1接收时CRCOK        发送时完成       清除溢出错误
    字节E：    IrqParam2       FIFO自动填充，超出阈值SX1211_FIFO_THRESH发送，PLL引脚输出，清除RSSI_IRQ标志
    字节F：    RssiIrqThresh   RSSI中断阈值

    字节10：RxParam1       滤波器设置，见3.4.4
    字节11：RxParam2       滤波器设置
    字节12：RxParam3       开启位同步，开启同步字，同步字2字节，容错1位
    字节13：Res19          保留
    字节14：RssiValue      RSSI（只读）
    字节15：RxParam6       OOK检测阈值，见3.4.10.2

    字节16：SyncByte1      同步字1
    字节17：SyncByte2      同步字2
    字节18：SyncByte3      同步字3
    字节19：SyncByte4      同步字4

    字节1A：TxParam            发送功率 -8dB
    字节1B：OscParam       CLKOUT开启，约200K
    字节1C：PktParam1      无曼彻斯特，载荷长度3F
    字节1D：NodeAddr       本机地址
    字节1E：PktParam3      变长数据，前导3字节，关闭白化，开启CRC，无地址校验
    字节1F：PktParam4      CRC错误自动清除，缓存默认写
*/


static uint8 SX1211_PreSetCfg[32][2] = {        // 预置SX1211配置字
    {0x00, 0x10},       // McParam1         模式1
    {0x02, 0x8C},       // McParam2         模式2
    {0x04, 0x03},       // Fdev             采样频率    自动生成        默认值
    {0x06, 0x07},       // Bitrate          传输速率    自动生成        默认值
    {0x08, 0x0C},       // OokFloorThresh   OOK设置       自动生成        默认值
    {0x0A, 0xC1},       // McParam6         FIFO设置
    {0x0C, 0x7D},       // R1                           自动生成
    {0x0E, 0x64},       // P1                           自动生成
    {0x10, 0x14},       // S1                           自动生成
    {0x12, 0x8F},       // R2                           自动生成
    {0x14, 0x72},       // P2                           自动生成
    {0x16, 0x41},       // S2                           自动生成
    {0x18, 0x38},       // PaRamp           OOK跳变时间                 默认值
    {0x1A, 0x09},       // IrqParam1        IRQ控制1
    {0x1C, 0x0D},       // IrqParam2        IRQ控制2                  默认值
    {0x1E, 0x00},       // RssiIrqThresh    RSSI阈值                  默认值
    {0x20, 0xA3},       // RxParam1                                     默认值
    {0x22, 0x38},       // RxParam2                                     默认值
    {0x24, 0x2A},       // RxParam3         接收控制3

    {0x26, 0x07},       // Res19            保留
    {0x68, 0x00},       // RssiValue        RSSI值,只读

    {0x2A, 0x00},       // RxParam6                                     默认值
    {0x2C, 0x25},       // SyncByte1        同步字1
    {0x2E, 0xC4},       // SyncByte2        同步字2
    {0x30, 0x00},       // SyncByte3        同步字3
    {0x32, 0x00},       // SyncByte4        同步字4

    {0x34, 0x7E},       // TxParam          发送滤波、功率
    {0x36, 0xFC},       // OscParam         晶振输出
    {0x38, 0x3F},       // PktParam1        包长度                     默认值
    {0x3A, 0x00},       // NodeAddr         本机地址                    默认值
    {0x3C, 0xC8},       // PktParam3        包控制1
    {0x3E, 0x00}        // PktParam4        包控制2
};



uint8 SX1211_MODE;                          // 工作模式，分为休眠、待机、就绪、接收、发送
uint8 SX1211_DATA_MODE;                     // 数据模式，分为IO、字节、数据包
bool SX1211_in_TX;                          // SX1211 发送中
bool SX1211_in_RX;                          // SX1211 接收中
bool SX1211_in_FSK;                     // SX1211 FSK模式



uint8 initiate_SX1211 (void)                                            // 初始化SX1211模块
{
    MCU_IO_INPUT (HAL_SX1211_CLKOUT_PORT, HAL_SX1211_CLKOUT_PIN, MCU_IO_PULLOFF);   // CLKOUT 为输入。SX1211启动时间约10mS，此时不可初始化，等待CLKOUT信号
    SX1211_SPI_Setup ();                                                    // 配置SX1211 SPI、DMA0

    TA2CCR0 = ((uint32)HAL_CPU_CLOCK_KHz *5) /4;                            // 定时10ms
    TA2CTL = TASSEL_2 + ID_3 + MC_1 + TAIE + TACLR;                         // SMCLK/8, up mode, interrupt disabled
    __low_power_mode_0();                                                   // LPM0休眠
    TA2CTL = 0;                                                             // 关闭TA2计时
    TA2CCTL0 = 0;

    if (MCU_IO_GET(HAL_SX1211_CLKOUT_PORT, HAL_SX1211_CLKOUT_PIN) == 0x00)  // 若10毫秒后未检测到CLKOUT，查找芯片
    {
            uint8 _check[2];
            SX1211_PreSetCfg [Res19][0] |= SX1211_CFG_READ;                 // 读操作
            SX1211_Read_CFG_SPI ((uint32)&SX1211_PreSetCfg[Res19],(uint32)_check, 2);   // 读取保留字
            while (SX1211_SPI_BUSY)     __low_power_mode_0();               // 等待SPI通信完毕
            SX1211_PreSetCfg [Res19][0] &= ~SX1211_CFG_WRITE;               // 写操作

            if (_check[1] != 0x07)  return (FALSE);                         // 芯片不存在或故障，退出
    }   // 验证通过，继续初始化

    SX1211_Send_CFG_DMA ((uint32)&SX1211_PreSetCfg, sizeof(SX1211_PreSetCfg));  // 发送初始配置字

    halDigioIntSetEdge (&pinRadio_IRQ0, HAL_DIGIO_INT_RISING_EDGE);         // pinRadio_IRQ0上升沿触发
    halDigioIntConnect (&pinRadio_IRQ0, IRQ0_isr);                          // 初始化中断，建立连接
    halDigioIntClear (&pinRadio_IRQ0);                                      // 清中断标志
    halDigioIntEnable (&pinRadio_IRQ0);                                     // 开启中断
    halDigioIntSetEdge (&pinRadio_IRQ1, HAL_DIGIO_INT_RISING_EDGE);         // pinRadio_IRQ1上升沿触发
    halDigioIntConnect (&pinRadio_IRQ1, IRQ1_isr);                          // 初始化中断，建立连接
    halDigioIntClear (&pinRadio_IRQ1);                                      // 清中断标志
    halDigioIntEnable (&pinRadio_IRQ1);                                     // 开启中断


//  初始化变量
    SX1211_MODE = SX1211_MODE_STDBY;                                        // 初使工作模式：待机
    SX1211_DATA_MODE = SX1211_DATA_MODE_PACKET;                             // 初使数据模式：数据包模式
    SX1211_in_TX = FALSE;                                                   // 未发送
    SX1211_in_RX = FALSE;                                                   // 未接收
    SX1211_in_FSK = TRUE;                                                   // FSK模式

    return (TRUE);
}





//  McParam1
uint8 SX1211_Operating_Mode (uint8 _value)                                  // SX1211工作模式选择
/*  参数：
#define SX1211_MODE_SLEEP                       0x00
#define SX1211_MODE_STDBY                       0x20                        d
#define SX1211_MODE_FS                          0x40
#define SX1211_MODE_RX                          0x60
#define SX1211_MODE_TX                          0x80
*/
{
    if ((_value | SX1211_MODE_MASK)!= SX1211_MODE_MASK)     return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [McParam1][0] &= ~SX1211_CFG_WRITE;                    // 写操作
    SX1211_PreSetCfg [McParam1][1] &= ~SX1211_MODE_MASK;                    // 清除原值
    SX1211_PreSetCfg [McParam1][1] |= _value;                               // 置新值

    SX1211_MODE = _value;                                                   // 更新状态

    if (_value == SX1211_MODE_TX)   SX1211_in_TX = TRUE;
    else    SX1211_in_TX = FALSE;
    if (_value == SX1211_MODE_RX)   SX1211_in_RX = TRUE;
    else    SX1211_in_RX = FALSE;                                           // 标志位

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                   // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[McParam1], 2);           // 发送配置字

    return (TRUE);
}


uint8 SX1211_Freq_Select (uint8 _value)                                     // 跳频选择，1或2
/*  参数：
#define SX1211_FREQ_1                           0x00                        d
#define SX1211_FREQ_2                           0x01
*/
{
    if ((_value | SX1211_FREQ_MASK)!= SX1211_FREQ_MASK)     return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [McParam1][0] &= ~SX1211_CFG_WRITE;                    // 写操作
    SX1211_PreSetCfg [McParam1][1] &= ~SX1211_FREQ_MASK;                    // 清除原值
    SX1211_PreSetCfg [McParam1][1] |= _value;                               // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[McParam1], 2);           // 发送配置字

    return (TRUE);
}


//  McParam2
uint8 SX1211_DATA_Mode (uint8 _value)                                       // SX1211数据模式
/*  参数：
#define SX1211_DATA_MODE_IO                     0x00                        d
#define SX1211_DATA_MODE_BYTE                   0x20
#define SX1211_DATA_MODE_PACKET                 0x04
*/
{
    if ((_value | SX1211_DATA_MODE_MASK)!= SX1211_DATA_MODE_MASK)       return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [McParam2][0] &= ~SX1211_CFG_WRITE;                    // 写操作
    SX1211_PreSetCfg [McParam2][1] &= ~SX1211_DATA_MODE_MASK;               // 清除原值
    SX1211_PreSetCfg [McParam2][1] |= _value;                               // 置新值

    SX1211_DATA_MODE = _value;                                              // 更新数据模式

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[McParam2], 2);           // 发送配置字

    return (TRUE);
}


uint8 SX1211_Modulation_Select (uint8 _value)                               // 调制模式选择
/*  参数：
#define SX1211_MOD_OOK                          0x40
#define SX1211_MOD_FSK                          0x80                        d
*/
{
    if ((_value | SX1211_MOD_MASK)!= SX1211_MOD_MASK)       return (FALSE); // 参数超出范围，错误
    if (_value == SX1211_MOD_FSK)   SX1211_in_FSK = TRUE;                   // FSK模式
    else        SX1211_in_FSK = FALSE;                                      // OOK模式

    SX1211_PreSetCfg [McParam2][0] &= ~SX1211_CFG_WRITE;                    // 写操作
    SX1211_PreSetCfg [McParam2][1] &= ~SX1211_MOD_MASK;                     // 清除原值
    SX1211_PreSetCfg [McParam2][1] |= _value;                               // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[McParam2], 2);           // 发送配置字

    return (TRUE);
}


uint8 SX1211_IF_GAIN (uint8 _value)                                         // SX1211   IF增益
/*  参数：
#define SX1211_IF_GAIN_0                        0x00                        d
#define SX1211_IF_GAIN_M45                      0x01
#define SX1211_IF_GAIN_M90                      0x02
#define SX1211_IF_GAIN_M135                     0x03
*/
{
    if ((_value | SX1211_IF_GAIN_MASK)!= SX1211_IF_GAIN_MASK)       return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [McParam2][0] &= ~SX1211_CFG_WRITE;                    // 写操作
    SX1211_PreSetCfg [McParam2][1] &= ~SX1211_IF_GAIN_MASK;                 // 清除原值
    SX1211_PreSetCfg [McParam2][1] |= _value;                               // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[McParam2], 2);           // 发送配置字

    return (TRUE);
}


//  McParam6
uint8 SX1211_FIFO_Size (uint8 _value)                                       // FIFO尺寸
/*  参数：
#define SX1211_FIFO_SIZE_16                     0x00                        d
#define SX1211_FIFO_SIZE_32                     0x40
#define SX1211_FIFO_SIZE_48                     0x80
#define SX1211_FIFO_SIZE_64                     0xC0
*/
{
    if ((_value | SX1211_FIFO_SIZE_MASK)!= SX1211_FIFO_SIZE_MASK)       return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [McParam6][0] &= ~SX1211_CFG_WRITE;                    // 写操作
    SX1211_PreSetCfg [McParam6][1] &= ~SX1211_FIFO_SIZE_MASK;               // 清除原值
    SX1211_PreSetCfg [McParam6][1] |= _value;                               // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[McParam6], 2);           // 发送配置字

    return (TRUE);
}


uint8 SX1211_FIFO_Threshold (uint8 _value)                                  // SX1211   FIFO中断阈值    参数范围：0-3F， 单位1
/*  参数：
#define SX1211_FIFO_THRESH_15                   0x0F                        d
#define SX1211_FIFO_THRESH_31                   0x1F
#define SX1211_FIFO_THRESH_47                   0x2F
#define SX1211_FIFO_THRESH_63                   0x3F
*/
{
    if ((_value | SX1211_FIFO_THRESH_MASK)!= SX1211_FIFO_THRESH_MASK)       return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [McParam6][0] &= ~SX1211_CFG_WRITE;                    // 写操作
    SX1211_PreSetCfg [McParam6][1] &= ~SX1211_FIFO_THRESH_MASK;             // 清除原值
    SX1211_PreSetCfg [McParam6][1] |= _value;                               // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[McParam6], 2);           // 发送配置字

    return (TRUE);
}


//  PaRamp
uint8 SX1211_RA_Ramp (uint8 _value)                                         // SX1211   OOK电平变化时间
/*  参数：
#define SX1211_PA_RAMP_3                        0x00
#define SX1211_PA_RAMP_8                        0x08
#define SX1211_PA_RAMP_15                       0x10
#define SX1211_PA_RAMP_23                       0x18                        d
*/
{
    if ((_value | SX1211_PA_RAMP_MASK)!= SX1211_PA_RAMP_MASK)       return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [PaRamp][0] &= ~SX1211_CFG_WRITE;                      // 写操作
    SX1211_PreSetCfg [PaRamp][1] &= ~SX1211_PA_RAMP_MASK;                   // 清除原值
    SX1211_PreSetCfg [PaRamp][1] |= _value;                                 // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[PaRamp], 2);             // 发送配置字

    return (TRUE);
}


//  IrqParam1
uint8 SX1211_IRQ0_Select (uint8 _value)                                     // IRQ0选择
/*  参数：
//  ------------    IRQ0：IO模式   RX  ------------        d: SYNC         注：IRQ0仅用于RX模式
#define SX1211_IRQ0_IO_SYNC                     0x00                        d
#define SX1211_IRQ0_IO_RSSI                     0x40
//  ------------    IRQ0：BYTE模式 RX  ------------        d: NULL         注：IRQ0仅用于RX模式
#define SX1211_IRQ0_BYTE_NULL                   0x00                        d
#define SX1211_IRQ0_BYTE_WRITE                  0x40
#define SX1211_IRQ0_BYTE_EMPTY                  0x80                    // 也适用于待机模式
#define SX1211_IRQ0_BYTE_SYNC                   0xC0
//  ------------    IRQ0：PACKET模式   RX  --------        d: PKTREADY     注：IRQ0仅用于RX模式
#define SX1211_IRQ0_PACKET_PKTREADY             0x00                        d
#define SX1211_IRQ0_PACKET_WRITE                0x40
#define SX1211_IRQ0_PACKET_EMPTY                0x80                    // 也适用于待机模式
#define SX1211_IRQ0_PACKET_SYNC                 0xC0                    //  Sync or Adrs_match
*/
{
    if ((_value | SX1211_IRQ0_RX_MASK)!= SX1211_IRQ0_RX_MASK)       return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [IrqParam1][0] &= ~SX1211_CFG_WRITE;                   // 写操作
    SX1211_PreSetCfg [IrqParam1][1] &= ~SX1211_IRQ0_RX_MASK;                // 清除原值
    SX1211_PreSetCfg [IrqParam1][1] |= _value;                              // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[IrqParam1], 2);          // 发送配置字

    return (TRUE);
}


uint8 SX1211_IRQ1_Select (uint8 _value)                                     // IRQ1选择
/*  参数：
//  ------------    IRQ1：IO模式   TX  RX  --------
//  任何值均为DCLK
//  ------------    IRQ1：BYTE模式 RX  ------------        d: NULL
#define SX1211_IRQ1_BYTE_NULL                   0x00                        d
#define SX1211_IRQ1_BYTE_FULL                   0x10                    // 也适用于待机模式
#define SX1211_IRQ1_BYTE_RSSI                   0x20
#define SX1211_IRQ1_BYTE_FIFOTHR                0x30                    // 也适用于待机模式
//  ------------    IRQ1：PACKET模式   RX  --------        d: CRCOK
#define SX1211_IRQ1_PACKET_CRCOK                0x00                        d
#define SX1211_IRQ1_PACKET_FULL                 0x10                    // 也适用于待机模式
#define SX1211_IRQ1_PACKET_RSSI                 0x20
#define SX1211_IRQ1_PACKET_FIFOTHR              0x30                    // 也适用于待机模式
--------------------------------------------------------------------------------
//  ------------    IRQ1：BYTE、PACKET模式  TX  ----        d: FULL
//  #define SX1211_IRQ1_BYTE_FULL                   0x00        重定义     d
#define SX1211_IRQ1_BYTE_TXDONE                 0x08
*/
{
    if ((_value | SX1211_IRQ1_MASK)!= SX1211_IRQ1_MASK)     return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [IrqParam1][0] &= ~SX1211_CFG_WRITE;                   // 写操作
    SX1211_PreSetCfg [IrqParam1][1] &= ~SX1211_IRQ1_MASK;                   // 清除原值
    SX1211_PreSetCfg [IrqParam1][1] |= _value;                              // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[IrqParam1], 2);          // 发送配置字

    return (TRUE);
}


uint8 SX1211_FIFO_Status (void)                                             // FIFO 状态读取
/*  参数：
#define SX1211_FIFO_FULL                        0x04
#define SX1211_FIFO_EMPTY                       0x02
#define SX1211_FIFO_OVERRUN                     0x01
*/
{
    uint8   _FIFO_Stat[2];

    SX1211_PreSetCfg [IrqParam1][0] |= SX1211_CFG_READ;                     // 读操作

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Read_CFG_SPI ((uint32)&SX1211_PreSetCfg[IrqParam1],(uint32)_FIFO_Stat, 2);   // 发送配置字
    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕

    return (_FIFO_Stat[1] &0x07);                                           // 返回状态
}


//  IrqParam2
uint8 SX1211_FIFO_Fill (uint8 _value)                                       // SX1211   FIFO写入
/*  参数：
#define SX1211_FIFOFILL_AUTO                    0x00                    // 检测到同步字即开始                d
#define SX1211_FIFOFILL_MANU                    0x80                    // FIFOFILL 控制
--------------------------------------------------------------------------------
#define SX1211_FIFOFILL_STOP                    0x00                    // 停止。设定AUTO位时此位无效      d
#define SX1211_FIFOFILL_START                   0x40                    // 开始。设定AUTO位时此位无效
*/
{
    if ((_value | SX1211_FIFOFILL_MASK)!= SX1211_FIFOFILL_MASK)     return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [IrqParam2][0] &= ~SX1211_CFG_WRITE;                   // 写操作
    SX1211_PreSetCfg [IrqParam2][1] &= ~SX1211_FIFOFILL_MASK;               // 清除原值
    SX1211_PreSetCfg [IrqParam2][1] |= _value;                              // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[IrqParam2], 2);          // 发送配置字

    return (TRUE);
}


uint8 SX1211_TX_Start (uint8 _value)                                        // SX1211发送控制
/*  参数：
#define SX1211_TXSTART_BYTE_FULL                0x00                    // 字节模式，缓存满时发送。IRQ0 = EMPTY                         d
#define SX1211_TXSTART_BYTE_NOTEMPTY            0x10                    // 字节模式，缓存非空发送。IRQ0 = EMPTY
#define SX1211_TXSTART_PACKET_FULL              0x00                    // 数据包模式，缓存≥FIFO_THRESH时发送。IRQ1 = FIFOTHR           d
#define SX1211_TXSTART_PACKET_NOTEMPTY          0x10                    // 数据包模式，缓存非空发送。IRQ0 = EMPTY
*/
{
    if ((_value | SX1211_TXSTART_MASK)!= SX1211_TXSTART_MASK)       return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [IrqParam2][0] &= ~SX1211_CFG_WRITE;                   // 写操作
    SX1211_PreSetCfg [IrqParam2][1] &= ~SX1211_TXSTART_MASK;                // 清除原值
    SX1211_PreSetCfg [IrqParam2][1] |= _value;                              // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[IrqParam2], 2);          // 发送配置字

    return (TRUE);
}


void SX1211_RSSI_IRQ_Clear (void)                                           // 清RSSI_IRQ中断
{
    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[IrqParam2], 2);          // 发送配置字
    return;
}


uint8 SX1211_PLL_OUT (uint8 _value)                                         // PLL_LOCK 功能选择
/*
#define SX1211_PLL_LOCK_OUT                     0x01                        d
#define SX1211_PLL_LOCK_DIS                     0x00
*/
{
    if ((_value | SX1211_PLL_LOCK_MASK)!= SX1211_PLL_LOCK_MASK)     return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [IrqParam2][0] &= ~SX1211_CFG_WRITE;                   // 写操作
    SX1211_PreSetCfg [IrqParam2][1] &= ~SX1211_PLL_LOCK_MASK;               // 清除原值
    SX1211_PreSetCfg [IrqParam2][1] |= _value;                              // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[IrqParam2], 2);          // 发送配置字

    return (TRUE);
}


//  RssiIrqThresh
void SX1211_RSSI_IRQ_Thresh (uint8 _value)                                  // 写RSSI_IRQ阈值
{
    SX1211_PreSetCfg [RssiIrqThresh][0] &= ~SX1211_CFG_WRITE;               // 写操作
    SX1211_PreSetCfg [RssiIrqThresh][1] = _value;                           // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[RssiIrqThresh], 2);      // 发送配置字
    return;
}


//  RxParam3
uint8 SX1211_POLYP (uint8 _value)                                           // ？？？？？        仅用于OOK  RX模式
/*  参数：
#define SX1211_POLYP_OFF                        0x00                        d
#define SX1211_POLYP_ON                         0x80
*/
{
//  if (SX1211_in_FSK)  return (FALSE);                                     // 非OOK，直接退出
    if ((_value | SX1211_POLYP_MASK)!= SX1211_POLYP_MASK)       return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [RxParam3][0] &= ~SX1211_CFG_WRITE;                    // 写操作
    SX1211_PreSetCfg [RxParam3][1] &= ~SX1211_POLYP_MASK;                   // 清除原值
    SX1211_PreSetCfg [RxParam3][1] |= _value;                               // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[RxParam3], 2);           // 发送配置字

    return (TRUE);
}


uint8 SX1211_BITSYNC (uint8 _value)                                         // 位同步选择        仅用于Continuous   RX模式
/*  参数：
#define SX1211_BITSYNC_ON                       0x00                        d
#define SX1211_BITSYNC_OFF                      0x40
*/
{
//  if (SX1211_DATA_MODE != SX1211_DATA_MODE_IO)    return (FALSE);         // 非Continuous，直接退出
    if ((_value | SX1211_BITSYNC_MASK)!= SX1211_BITSYNC_MASK)       return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [RxParam3][0] &= ~SX1211_CFG_WRITE;                    // 写操作
    SX1211_PreSetCfg [RxParam3][1] &= ~SX1211_BITSYNC_MASK;                 // 清除原值
    SX1211_PreSetCfg [RxParam3][1] |= _value;                               // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[RxParam3], 2);           // 发送配置字

    return (TRUE);
}


uint8 SX1211_SYNC (uint8 _value)                                            // 字同步选择
/*  参数：
#define SX1211_SYNC_OFF                         0x00
#define SX1211_SYNC_1B                          0x20
#define SX1211_SYNC_2B                          0x28
#define SX1211_SYNC_3B                          0x30
#define SX1211_SYNC_4B                          0x38                        d
--------------------------------------------------------------------------------
#define SX1211_SYNC_TOL_0                       0x00                        d
#define SX1211_SYNC_TOL_1                       0x02
#define SX1211_SYNC_TOL_2                       0x04
#define SX1211_SYNC_TOL_3                       0x06
*/
{
//  if (SX1211_DATA_MODE != SX1211_DATA_MODE_PACKET)    return (FALSE);     // 非数据包，直接退出
    if ((_value | SX1211_SYNC_MASK)!= SX1211_SYNC_MASK)     return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [RxParam3][0] &= ~SX1211_CFG_WRITE;                    // 写操作
    SX1211_PreSetCfg [RxParam3][1] &= ~SX1211_SYNC_MASK;                    // 清除原值
    SX1211_PreSetCfg [RxParam3][1] |= _value;                               // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[RxParam3], 2);           // 发送配置字

    return (TRUE);
}


//  RssiValue
uint8 SX1211_GET_RSSI (void)                                                // 获取RSSI值
{
    uint8 _rssi[2];

    SX1211_PreSetCfg [RssiValue][0] |= SX1211_CFG_READ;                     // 读操作

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Read_CFG_SPI ((uint32)&SX1211_PreSetCfg[RssiValue],(uint32)_rssi, 2);    // 发送配置字
    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕

    return (_rssi[1]);                                                      // 返回RSSI值
}


//  SyncByte
void SX1211_SYNC_BYTE (uint32 _value)                                       // 设定同步字，高位为1，低位为4
{
    SX1211_PreSetCfg [SyncByte1][0] &= ~SX1211_CFG_WRITE;                   // 写操作
    SX1211_PreSetCfg [SyncByte2][0] &= ~SX1211_CFG_WRITE;                   // 写操作
    SX1211_PreSetCfg [SyncByte3][0] &= ~SX1211_CFG_WRITE;                   // 写操作
    SX1211_PreSetCfg [SyncByte4][0] &= ~SX1211_CFG_WRITE;                   // 写操作


    SX1211_PreSetCfg [SyncByte1][1] = BREAK_UINT32 (_value, 3);
    SX1211_PreSetCfg [SyncByte2][1] = BREAK_UINT32 (_value, 2);
    SX1211_PreSetCfg [SyncByte3][1] = BREAK_UINT32 (_value, 1);
    SX1211_PreSetCfg [SyncByte4][1] = BREAK_UINT32 (_value, 0);

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[SyncByte1], 8);          // 发送配置字
    return;
}


//  TxParam
uint8 SX1211_POUT_Select (uint8 _value)                                     // 输出功率选择
/*  参数：
#define SX1211_POUT_13                          0x00
#define SX1211_POUT_10                          0x02                        d
#define SX1211_POUT_7                           0x04
#define SX1211_POUT_4                           0x06
#define SX1211_POUT_1                           0x08
#define SX1211_POUT_M2                          0x0A
#define SX1211_POUT_M5                          0x0C
#define SX1211_POUT_M8                          0x0E
*/
{
    if ((_value | SX1211_POUT_MASK)!= SX1211_POUT_MASK)     return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [TxParam][0] &= ~SX1211_CFG_WRITE;                     // 写操作
    SX1211_PreSetCfg [TxParam][1] &= ~SX1211_POUT_MASK;                     // 清除原值
    SX1211_PreSetCfg [TxParam][1] |= _value;                                // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[TxParam], 2);            // 发送配置字

    return (TRUE);
}


//  OscParam
uint8 SX1211_CLKOUT (uint8 _value)                                          // 输出时钟
/*  参数：
#define SX1211_CLKOUT_OFF                       0x00
#define SX1211_CLKOUT_ON                        0x80                        d
*/
{
    if ((_value | SX1211_CLKOUT_MASK)!= SX1211_CLKOUT_MASK)     return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [OscParam][0] &= ~SX1211_CFG_WRITE;                    // 写操作
    SX1211_PreSetCfg [OscParam][1] &= ~SX1211_CLKOUT_MASK;                  // 清除原值
    SX1211_PreSetCfg [OscParam][1] |= _value;                               // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[OscParam], 2);           // 发送配置字

    return (TRUE);
}


//  PktParam1
uint8 SX1211_Manchester (uint8 _value)                                      // 曼彻斯特编码
/*  参数：
#define SX1211_MANCH_OFF                        0x00                        d
#define SX1211_MANCH_ON                         0x80
*/
{
    if ((_value | SX1211_MANCH_MASK)!= SX1211_MANCH_MASK)       return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [PktParam1][0] &= ~SX1211_CFG_WRITE;                   // 写操作
    SX1211_PreSetCfg [PktParam1][1] &= ~SX1211_MANCH_MASK;                  // 清除原值
    SX1211_PreSetCfg [PktParam1][1] |= _value;                              // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[PktParam1], 2);          // 发送配置字

    return (TRUE);
}


uint8 SX1211_Length (uint8 _value)                                          // 数据包长度    （变长模式下，RX时为最大数据长度，TX时无定义）
{
    if (_value >=64)        return (FALSE);                                 // 参数超出范围，错误

    SX1211_PreSetCfg [PktParam1][0] &= ~SX1211_CFG_WRITE;                   // 写操作
    SX1211_PreSetCfg [PktParam1][1] &= ~SX1211_LENGTH_MASK;                 // 清除原值
    SX1211_PreSetCfg [PktParam1][1] |= _value;                              // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[PktParam1], 2);          // 发送配置字

    return (TRUE);
}


//  NodeAddr
void SX1211_Address (uint8 _value)                                          // 数据包地址字节
{
    SX1211_PreSetCfg [NodeAddr][0] &= ~SX1211_CFG_WRITE;                    // 写操作
    SX1211_PreSetCfg [NodeAddr][1] = _value;                                // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[NodeAddr], 2);           // 发送配置字

    return;
}


//  PktParam3
uint8 SX1211_Length_Set (uint8 _value)                                      // 数据包长度设定
/*  参数：
#define SX1211_LENGTH_FIX                       0x00                        d
#define SX1211_LENGTH_VAR                       0x80
*/
{
//  if (SX1211_DATA_MODE != SX1211_DATA_MODE_PACKET)    return (FALSE);     // 非数据包，直接退出
    if ((_value | SX1211_LENGTH_SET_MASK)!= SX1211_LENGTH_SET_MASK)     return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [PktParam3][0] &= ~SX1211_CFG_WRITE;                   // 写操作
    SX1211_PreSetCfg [PktParam3][1] &= ~SX1211_LENGTH_SET_MASK;             // 清除原值
    SX1211_PreSetCfg [PktParam3][1] |= _value;                              // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[PktParam3], 2);          // 发送配置字

    return (TRUE);
}


uint8 SX1211_Preamble (uint8 _value)                                        // 前导长度
/*  参数：
#define SX1211_PREA_1B                          0x00
#define SX1211_PREA_2B                          0x20
#define SX1211_PREA_3B                          0x40                        d
#define SX1211_PREA_4B                          0x60
*/
{
    if ((_value | SX1211_PREA_MASK)!= SX1211_PREA_MASK)     return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [PktParam3][0] &= ~SX1211_CFG_WRITE;                   // 写操作
    SX1211_PreSetCfg [PktParam3][1] &= ~SX1211_PREA_MASK;                   // 清除原值
    SX1211_PreSetCfg [PktParam3][1] |= _value;                              // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[PktParam3], 2);          // 发送配置字

    return (TRUE);
}


uint8 SX1211_Whitening (uint8 _value)                                       // 白化
/*  参数：
#define SX1211_WHITENING_OFF                    0x00                        d
#define SX1211_WHITENING_ON                     0x10
*/
{
//  if (SX1211_DATA_MODE != SX1211_DATA_MODE_PACKET)    return (FALSE);     // 非数据包，直接退出
    if ((_value | SX1211_WHITENING_MASK)!= SX1211_WHITENING_MASK)       return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [PktParam3][0] &= ~SX1211_CFG_WRITE;                   // 写操作
    SX1211_PreSetCfg [PktParam3][1] &= ~SX1211_WHITENING_MASK;              // 清除原值
    SX1211_PreSetCfg [PktParam3][1] |= _value;                              // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[PktParam3], 2);          // 发送配置字

    return (TRUE);
}


uint8 SX1211_Address_FILT (uint8 _value)                                    // 数据包地址过滤
/*  参数：
#define SX1211_ADDR_FILT_OFF                    0x00                        d
#define SX1211_ADDR_FILT_ON                     0x02
#define SX1211_ADDR_FILT_ON_0                   0x04                    // 地址或0x00
#define SX1211_ADDR_FILT_ON_0F                  0x06                    // 地址或0x00或0xFF
*/
{
    if ((_value | SX1211_ADDR_FILT_MASK)!= SX1211_ADDR_FILT_MASK)       return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [PktParam3][0] &= ~SX1211_CFG_WRITE;                   // 写操作
    SX1211_PreSetCfg [PktParam3][1] &= ~SX1211_ADDR_FILT_MASK;              // 清除原值
    SX1211_PreSetCfg [PktParam3][1] |= _value;                              // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[PktParam3], 2);          // 发送配置字

    return (TRUE);
}


uint8 SX1211_CRC (uint8 _value)                                             // 数据包CRC校验
/*  参数：
#define SX1211_CRC_OFF                          0x00
#define SX1211_CRC_ON                           0x08                        d
*/
{
//  if (SX1211_DATA_MODE != SX1211_DATA_MODE_PACKET)    return (FALSE);     // 非数据包，直接退出
    if ((_value | SX1211_CRC_MASK)!= SX1211_CRC_MASK)       return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [PktParam3][0] &= ~SX1211_CFG_WRITE;                   // 写操作
    SX1211_PreSetCfg [PktParam3][1] &= ~SX1211_CRC_MASK;                    // 清除原值
    SX1211_PreSetCfg [PktParam3][1] |= _value;                              // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[PktParam3], 2);          // 发送配置字

    return (TRUE);
}


//  PktParam4
uint8 SX1211_CRC_AUTOCLR (uint8 _value)                                     // CRC校验失败直接清除
/*  参数：
#define SX1211_CRC_AUTOCLR_ON                   0x00                        d
#define SX1211_CRC_AUTOCLR_OFF                  0x80
*/
{
//  if (SX1211_DATA_MODE != SX1211_DATA_MODE_PACKET)    return (FALSE);     // 非数据包，直接退出
    if ((_value | SX1211_CRC_AUTOCLR_MASK)!= SX1211_CRC_AUTOCLR_MASK)       return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [PktParam4][0] &= ~SX1211_CFG_WRITE;                   // 写操作
    SX1211_PreSetCfg [PktParam4][1] &= ~SX1211_CRC_AUTOCLR_MASK;            // 清除原值
    SX1211_PreSetCfg [PktParam4][1] |= _value;                              // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[PktParam4], 2);          // 发送配置字

    return (TRUE);
}


uint8 SX1211_FIFO_Standby (uint8 _value)                                    // 待机模式FIFO
/*  参数：
#define SX1211_STDBY_FIFO_WRITE                 0x00                        d
#define SX1211_STDBY_FIFO_READ                  0x40
*/
{
//  if (SX1211_DATA_MODE == SX1211_DATA_MODE_IO)    return (FALSE);         // Continuous，直接退出
    if ((_value | SX1211_STDBY_FIFO_MASK)!= SX1211_STDBY_FIFO_MASK)     return (FALSE); // 参数超出范围，错误

    SX1211_PreSetCfg [PktParam4][0] &= ~SX1211_CFG_WRITE;                   // 写操作
    SX1211_PreSetCfg [PktParam4][1] &= ~SX1211_STDBY_FIFO_MASK;             // 清除原值
    SX1211_PreSetCfg [PktParam4][1] |= _value;                              // 置新值

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[PktParam4], 2);          // 发送配置字

    return (TRUE);
}





uint8 SX1211_TX_Bytes (uint32 _address, uint8 _length)                      // 发送数据，地址指针和长度
{
    //应先清空缓存
    if (SX1211_DATA_MODE == SX1211_DATA_MODE_IO)    return (FALSE);         // 数据模式为IO，直接退出
    if (_length >64)    _length = 64;                                       // 长度最多支持64字节

    // 中断   ，模式切换

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Send_Bytes_DMA (_address, _length);                              // 发送数据

    return (TRUE);
}


void SX1211_RX_Bytes (uint8 *rxBuffer)                                      // 接收缓存中数据。因无需使用DMA，因此参数类型与众不同。事实上，该程序在将FIFO的值读入rxBuffer的同时，将rxBuffer的原值写入了FIFO
{
    if (SX1211_DATA_MODE == SX1211_DATA_MODE_IO)
    {   Rec_num = 0;    return;     }                                       // 数据模式为IO，直接退出

    uint8 PKT_Length;

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
    SX1211_Read_Bytes_SPI ((uint32)rxBuffer,(uint32)&PKT_Length, 1);            // 随便发一个什么数，接收数据长度
    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕

    *rxBuffer = PKT_Length;
    rxBuffer++;

    SX1211_Read_Bytes_SPI ((uint32)rxBuffer,(uint32)rxBuffer, PKT_Length);  // 接收数据
    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // 等待之前的SPI发送完毕
}



HAL_ISR_FUNCTION(TA2_time_up,TIMER2_A1)
{
    TA2CTL &= ~TAIFG;
    __low_power_mode_off_on_exit();             // Clear CPUOFF bit from 0(SR)
}
