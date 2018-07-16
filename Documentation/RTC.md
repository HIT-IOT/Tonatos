# RTC - 实时时钟

## 综述
系统实时时钟，未使用DMA，也未使用IIC中断，使用IIC和RX6110通信。

注意RX6110上电40ms后方可操作，读取VLF-bit。如为1，代表时钟丢失，再等1秒后初始化；如为0，可直接操作。

本模块未考虑中断的影响。其它中断程序可能会打断IIC通信时序。如RTC通信有问题，可考虑屏蔽中断。

RX6110的配置位和MSP430的IIC模块较复杂，下面列出其使用说明。

尤其是MSP430芯片的IIC模块有较大改动，如使用不当，会导致IIC通信锁死，SCL拉低。具体原理参见手册图26-12、26-13。

## RX6110 特性
内置16字节RAM，自动的备用供电管理。

分两种工作模式：
1. VDD电压高于1.6V时为正常模式，可正常通信；
2. VDD电压低于1.6V为时钟维持模式，使用备用电源供电，通信禁止，IRQ2禁止

时钟维持最低电压：1.1V；  IIC通信最低电压：1.6V；     工作电压上限：5.5V；    最高耐受电压：6.5V；
时钟维持模式电流：130nA

第一次上电晶振起振需要0.3秒，最长为1秒
VDD上电后，至少等待40ms再通信。200ms后电容充满。
可输出32K/1K/1Hz的频率，其中3V输出32K时消耗电流至少为1.5uA
两路中断输出，IRQ1为开漏输出，一直有效；IRQ2为CMOS输出，1.6V以上有效

可支持SPI通信或IIC通信
IIC可支持400K速率，但SCL的高低电平时间不对称。如必须对称，则速率应降到333K以下
IIC地址：7位地址，0x64

IEA节点设计时已考虑相关硬件要求，不再赘述

## RX6110 功能
1. 计时，年月日时分秒，星期。2000-2099年，自动闰年。读取时自动锁存
2. IRQ1专属中断：（可用于断电唤醒）
        定时中断，指定年月日时分（注意无秒），持续触发电平
        每分/秒中断，脉冲触发电平持续7.813ms
        1Hz中断。注意输出为非对称方波
4. IRQ1、IRQ2共有中断：
        周期中断，244.14 us ~ 65535 hours，脉冲触发电平
        时钟输出
5. 其他功能：
        累计上电/断电时间
        时钟有效检测



RX6110 REG格式：
---------------------------------------------------------------------------------
| 0x10  |Second |---0---|          BCD          |        BCD    (00 - 59)       | *1
|-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
| 0x11  |Minute |---0---|          BCD          |        BCD    (00 - 59)       |
|-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
| 0x12  |Hour   |---0---|---0---|      BCD      |        BCD    (00 - 23)       |
|-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
| 0x13  |Week   |---0---|--Sat--|--Fri--|--Thu--|--Wed--|--Tue--|--Mon--|--Sun--| *2
|-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
| 0x14  |Day    |---0---|---0---|      BCD      |    BCD (01 - 28,29,30,31)     | *3
|-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
| 0x15  |Month  |---0---|---0---|---0---|      BCD              (01 - 12)       |
|-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
| 0x16  |Year   |              BCD              |        BCD    (00 - 99)       |
|-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
| 0x17  |-------------------------- Reserved (10101000) ------------------------| *4
|-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
| 0x18  |Aw,Min |   AE  |          BCD          |        BCD    (00 - 59)       | *5
|-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
| 0x19  |Aw,Hour|   AE  |---0---|      BCD      |        BCD    (00 - 23)       | *5
|-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
|       |Aw,Week|       |--Sat--|--Fri--|--Thu--|--Wed--|--Tue--|--Mon--|--Sun--|
| 0x1A  +-------+   AE  +-------+-------+-------+-------+-------+-------+-------| *5
|       |Aw,Day |       |---0---|      BCD      |    BCD (01 - 28,29,30,31)     |
|-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
| 0x1B  |TC0    |  128  |   64  |   32  |   16  |   8   |   4   |   2   |   1   |
|-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
| 0x1C  |TC1    | 32768 | 16384 |  8192 |  4096 |  2048 |  1024 |  512  |  256  |
|-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
| 0x1D  |Exten  | FSEL1 | FSEL0 |  USEL |   TE  |  WADA | TSEL2 | TSEL1 | TSEL0 |
|-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
| 0x1E  |Flag   |---0---|---0---|   UF  |   TF  |   AF  |---0---|  VLF  |---0---|
|-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
| 0x1F  |Ctrl   |---0---|  STOP |  UIE  |  TIE  |  AIE  |---0---| TBKON |  TBKE |
---------------------------------------------------------------------------------

*0: 错误的格式可能导致时钟错误。
*1: 写入时，内部计时器自动清零
*2: 只能一位为1
*3: 日期根据年月自动进位
*4: 必须写入0xA8
*5: AE如为1，则忽略该寄存器。若AE全为1，则每分钟触发一次。


---------------------------------------------------------------------------------
| 0x20  |       |                                                               |
|   ~   +  RAM  +                         128 Bits                              |
| 0x2F  |       |                                                               |
---------------------------------------------------------------------------------

---------------------------------------------------------------------------------
| 0x30  |---------------------------- CLOCK OFFSET -----------------------------|
|-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
| 0x31  |       |---0---|---0---|---0---|---1---| BK SON|BK SOFF|BK SMP1|BK SMP0|
|-------+-------+-------+-------+-------+-------+-------+-------+-------+-------|
| 0x32  |  IRQ  |---0---|---0---|---0---| TMREC |---0---| TMPIN |---0---| FOPIN0|
---------------------------------------------------------------------------------

定时中断： 0x18~0x1A、WADA、AF、AIE
        设置前，先将AIE清0。
        当输入的时间恰好满足闹钟定时器时，闹钟会等到下次匹配时再触发中断，而不是立即触发
    WADA: 选择0x1A功能
        0：      WEEK。可多选
        1：      DAY
    AF：为1表明产生中断。需手动清0，使IRQ1恢复。
    AIE：置1使能IRQ1输出

周期中断： 0x1B、0x1C、TE、TF、TIE、TSEL2、TSEL1、TSEL0、TBKON、TBKE、TMPIN
        设置前，先将TE清0
    从0x1B、0x1C中预设的值开始减到0，触发中断并开始下轮计时。
    0x1B+0x1C：单位个数，范围1-65535。当TE为1时读到的是瞬时值
    TSEL2+TSEL1+TSEL0：单位时长。 *：第一个计数周期达不到单位时间。               ************
        000:    每单位244.14us，最长15.99秒。       中断持续122us
        001:    每单位15.625ms，最长1023.9秒。  中断持续7.813ms                     ************
        010:    每单位1sec，    最长18.2小时。   中断持续7.813ms
        011:    每单位1min，    最长45.5天。    中断持续7.813ms
        100:    每单位1hour，   最长7.48年。    中断持续7.813ms
    TE：置1使能周期中断，并重新开始计时
    TF：为1表明产生过中断。需手动清0
    TIE：置1使能IRQ输出
    TBKON+TBKE：
        00： 一直计时
        01： 上电时计时
        10： 一直计时
        11： 电池供电时计时
    TMPIN:
        0：      输出到IRQ2
        1：      输出到IRQ1

每分/秒中断： USEL、UF、UIE
        设置前，先将UIE清0
    USEL: 0为秒，1为分
    UF：为1表明产生过中断。需手动清0
    UIE：置1使能IRQ1输出

1Hz中断：
    TMREC：置1使能1Hz输出到IRQ1

时钟输出： FSEL1、FSEL0、FOPIN0
        只列出IEA节点适用的配置
    FOPIN0：0输出到IRQ2，1输出到IRQ1（电池时仍有效）
    FSEL1+FSEL0：
        00： OFF (d)
        01： 1Hz
        10： 1024Hz
        11： 32768Hz(不要输出到IRQ2)

时钟有效检测： VLF
    当VLF为1时，说明时钟已经丢失，需要重新初始化

时钟停止： STOP
    STOP为1时，时钟停止，时钟相关中断停止；周期中断停止；
        32K、1K时钟输出保持，1Hz时钟停止

电压管理：BK SON、BK SOFF、BK SMP1、BK SMP0
    BK SON + BK SOFF：
        00： 使用备用电池
        01： 直接供电，无备用电源
        10： 不检测电压，不自动切换备用模式，电压需保持在1.6V以上
        11： 使用备用电池，省电模式
    BKSON + BKSOFF + BKSMP1 + BKSMP0：
        0000：   正常供电模式一直检测电压    时钟维持模式每秒检测2ms       充电关闭2ms（充998ms，检测2秒）
        0001：   正常供电模式一直检测电压    时钟维持模式每秒检测2ms       充电关闭16ms
        0010：   正常供电模式一直检测电压    时钟维持模式每秒检测2ms       充电关闭128ms
        0011：   正常供电模式一直检测电压    时钟维持模式每秒检测2ms       充电关闭256ms（虽只用了2ms检测，但其余的254ms也没向备用电池充电）
        0100：   正常供电模式每秒检测2ms       时钟维持模式每秒检测2ms       永不向备用电池充电
        0101：   正常供电模式每秒检测16ms  时钟维持模式每秒检测2ms       永不向备用电池充电
        0110：   正常供电模式每秒检测128ms 时钟维持模式每秒检测2ms       永不向备用电池充电
        0111：   正常供电模式每秒检测256ms 时钟维持模式每秒检测2ms       永不向备用电池充电
        10xx：   正常供电模式永不检测电压    时钟维持模式永不检测电压    备用电池充电一直开启
        1100：   正常供电模式每秒检测2ms       时钟维持模式每秒检测2ms       充电关闭2ms（该配置最省电）
        1101：   正常供电模式每秒检测16ms  时钟维持模式每秒检测2ms       充电关闭16ms
        1110：   正常供电模式每秒检测128ms 时钟维持模式每秒检测2ms       充电关闭128ms
        1111：   正常供电模式每秒检测256ms 时钟维持模式每秒检测2ms       充电关闭256ms





注意：
MSP430芯片的IIC模块有较大改动，初衷是提高IIC效率，但复杂度也提高了。
特性： 7/10位地址，支持广播，支持多主机，支持4个从地址，支持DMA，支持LPM4唤醒，支持自动计数并停止。
改动： 将启动位和停止位与发送/接收绑定，不可单独操作。使用前一定要弄清其具体原理，参见手册图26-12、26-13。错误的使用会导致总线卡死！！！
        ACK不再单独检测，而与发送绑定！！！


注意：
FR59xx的库eusci_b_i2c如使用不当，会导致IIC通信锁死，SCL拉低。
因此IIC通信请仅使用以下基本指令：

uint16_t EUSCI_B_I2C_isBusBusy(uint16_t baseAddress)                // 判断总线是否空闲，双线是否拉高
//!         - \b EUSCI_B_I2C_BUS_BUSY
//!         - \b EUSCI_B_I2C_BUS_NOT_BUSY

uint16_t EUSCI_B_I2C_masterIsStopSent(uint16_t baseAddress)         // 判断停止位是否发出。需要先执行EUSCI_B_I2C_masterMultiByteReceiveStop
//!         - \b EUSCI_B_I2C_STOP_SEND_COMPLETE
//!         - \b EUSCI_B_I2C_SENDING_STOP

uint16_t EUSCI_B_I2C_masterIsStartSent(uint16_t baseAddress)        // 判断起始位是否发出。需要先执行EUSCI_B_I2C_masterSendStart
//!         - \b EUSCI_B_I2C_START_SEND_COMPLETE
//!         - \b EUSCI_B_I2C_SENDING_START



void EUSCI_B_I2C_setMode(uint16_t baseAddress, uint8_t mode)        // 设置发送/接收。需在EUSCI_B_I2C_masterSendStart前调用
//!        - \b EUSCI_B_I2C_TRANSMIT_MODE [Default]
//!        - \b EUSCI_B_I2C_RECEIVE_MODE

void EUSCI_B_I2C_masterSendStart(uint16_t baseAddress)              // 发送起始位
                                                                    // 如为主动发送模式，忽略TXBUF内容(重启不忽略)，自动发送START标志、寻址字节，不包括ACK。向TXBUF写入下一个发送字节时才检查ACK。检查完ACK后STT清零，TXIFG置1。
                                                                    // 如为主动接收模式，自动发送START标志、寻址字节，包括ACK。ACK结束后STT清零。若无ACK，NACKIFG置1。
                                                                            // 若ACK正常，随后自动接收第一个字节，接收完毕后RXIFG置1
                                                                            // 若无ACK，取消接收，NACKIFG置1；此时发送停止位将直接发送停止；此时发送启动位将直接发送START标志和寻址字节。

void EUSCI_B_I2C_masterMultiByteReceiveStop(uint16_t baseAddress)   // 发送停止位
                                                                    // 如为主动发送模式，若正在发送，则在当前字节发送完毕后发送停止位；若正在空闲，则在下一字节发送完毕后发送停止位；若上一字节的ACK错误，则立即发送停止位
                                                                    // 如为主动接收模式，若在接收中，则在当前字节接收完毕后发送NACK，并发送停止位；

void EUSCI_B_I2C_slaveDataPut(uint16_t baseAddress, uint8_t transmitData)   // 发送一个字节
                                                                    // 仅适用发送模式。首先检测上一个字节的ACK：
                                                                            // 如未通过，则取消发送，NACKIFG置1；此时发送停止位将直接发送停止；此时发送启动位将直接发送START标志和寻址字节。
                                                                            // 如通过，TXIFG置1，可以写入下个数据，此时仍在发送中，只是TXBUF为空。此时发送停止位将在下一字节发送完毕后发送。
                                                                            // 在发送字节时若发送启动位，则在本字节发送完毕后自动检查ACK，并发送START标志和寻址字节。

uint8_t EUSCI_B_I2C_slaveDataGet(uint16_t baseAddress)              // 接收一个字节。
                                                                    // 仅适用接收模式。当检测到RXIFG为1时调用，读取RXBUF内容。
                                                                            // 在接收字节时若发送启动位，则在本字节发送完毕后自动发送NACK，并发送START标志和寻址字节。


void EUSCI_B_I2C_clearInterruptFlag(uint16_t baseAddress, uint16_t mask)        // 清标志位
uint16_t EUSCI_B_I2C_getInterruptStatus(uint16_t baseAddress, uint16_t mask)    // 读标志位
//!        - \b EUSCI_B_I2C_NAK_INTERRUPT - Not-acknowledge interrupt
//!        - \b EUSCI_B_I2C_STOP_INTERRUPT - STOP condition interrupt
//!        - \b EUSCI_B_I2C_START_INTERRUPT - START condition interrupt
//!        - \b EUSCI_B_I2C_TRANSMIT_INTERRUPT0 - Transmit interrupt0
//!        - \b EUSCI_B_I2C_RECEIVE_INTERRUPT0 - Receive interrupt0



最好等待每个操作完成后再执行后面的操作。调试IIC时一定用示波器观察波形是否正确！！！
例程：写入：

    EUSCI_B_I2C_clearInterruptFlag (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_NAK_INTERRUPT); // 清NACK标志

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                           // 发送写入，自动发送START标志、寻址字节，不包括ACK。

    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                        // 写入第一字节：地址00
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                                // 此时ACK已检测完毕，第一字节已写入移位寄存器并正在发送

    if (EUSCI_B_I2C_getInterruptStatus (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_NAK_INTERRUPT)) // 如寻址错误，芯片不存在，发送取消
    {
        EUSCI_B_I2C_masterMultiByteSendStop (HAL_BOARD_IIC_BASE);               // 立即发送停止位
        while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE);
    }
    else
    {
        EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x01);                    // 写1
        while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

        EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x02);                    // 写2
        while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

        EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x03);                    // 写3
        while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

        EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
        EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 重启
        EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x0F);                    // 写入第二地址0F
        while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕

        EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x04);                    // 写4
        while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

        EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x05);                    // 写5
        while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

        EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x06);                    // 写6
        while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

        EUSCI_B_I2C_masterMultiByteReceiveStop (HAL_BOARD_IIC_BASE);            // 此时6正在发送中，发送停止位。注意如果中间夹入中断，可能会导致停止位发不出去！！！
        while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE); // 读取 FlagReg
    }


例程：读取一字节：

    EUSCI_B_I2C_clearInterruptFlag (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_NAK_INTERRUPT); // 清NACK标志

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                           // 发送写入，自动发送START标志、寻址字节，不包括ACK。

    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                        // 写入第一字节：地址00
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                                // 此时ACK已检测完毕，第一字节已写入移位寄存器并正在发送

    if (EUSCI_B_I2C_getInterruptStatus (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_NAK_INTERRUPT)) // 如寻址错误，芯片不存在，发送取消
    {
        EUSCI_B_I2C_masterMultiByteSendStop (HAL_BOARD_IIC_BASE);               // 立即发送停止位
        while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE);
    }
    else
    {
        EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_MODE);
        EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 重启
        while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                                // 此时正在接收第一字节

        EUSCI_B_I2C_masterMultiByteReceiveStop (HAL_BOARD_IIC_BASE);            // 发送停止位
        while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE); // 读取 FlagReg
        data = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);                   // 读取RXBUF
    }


例程：读取多字节：

    EUSCI_B_I2C_clearInterruptFlag (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_NAK_INTERRUPT); // 清NACK标志

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                           // 发送写入，自动发送START标志、寻址字节，不包括ACK。

    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                        // 写入第一字节：地址00
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                                // 此时ACK已检测完毕，第一字节已写入移位寄存器并正在发送

    if (EUSCI_B_I2C_getInterruptStatus (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_NAK_INTERRUPT)) // 如寻址错误，芯片不存在，发送取消
    {
        EUSCI_B_I2C_masterMultiByteSendStop (HAL_BOARD_IIC_BASE);               // 立即发送停止位
        while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE);
    }
    else
    {
        EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_MODE);
        EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 重启
        while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                                // 此时正在接收第一字节

        while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0));    // 等待数据1
        data1 = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);

        while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0));    // 等待数据2
        data2 = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);

        while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0));    // 等待数据3
        data3 = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);

        EUSCI_B_I2C_masterMultiByteReceiveStop (HAL_BOARD_IIC_BASE);            // 发送停止位
        while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE); // 读取 FlagReg
        data4 = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);                  // 读取数据4
    }


接口、函数说明：
void rtc_power_on (void)：RTC上电。上电40ms后方可通信
void rtc_power_off (void)：关闭RTC供电，RTC使用备用电容低功耗运行。

以下程序调用前需确保RTC已上电40ms以上。

uint16 rtc_valid_check (void)：RTC有效性和状态检查
        节点上电后，要通过该程序得知RTC模块是否还保存着时钟。
            如时钟保持，则可以直接读取和同步；
            如时钟丢失，则需要通过网络重新同步RTC
void rtc_read (void)：读取年月日时分秒，应当用不到。
uint16 rtc_preset (void)：初始化RX6110芯片，配置内部寄存器，并运行
uint16 rtc_wake (uint32 _time)：设置唤醒时间。参数为毫秒，实际时间可能少0~15ms。_time不应超过1020秒，即1020000。
            如输入100000，表明100秒后唤醒节点。
                如此时节点断电，则唤醒并运行（C2中应有足够的电量）
                如此时节点运行中，
                    如此时节点C1电压超过2.7V，无影响继续运行
                    如此时节点C1电压低于2.7V，则将C1电压充至2.7V，程序无影响继续运行
            注意该功能如不关闭，则周期性唤醒。
uint16 rtc_wake_off (void)：关闭唤醒
uint16 rtc_sync (void)：同步RTC时钟。执行本指令后，IRQ2立即回到低电位，重新计时
uint16 rtc_sync_en (void)：在IRQ2上输出同步时钟。
uint16 rtc_sync_dis (void)：在IRQ2上关闭同步时钟。

void rtc_run (void)：计时，每1秒需调用1次
