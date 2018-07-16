#include <RTC.h>
#include <macros.h>
#include <msp430.h>
#include <i2c.h>
#include <hal.h>
RTCControl rtc;
static bool RTC_available = FALSE;                                      // RTC是否有效

void rtc_power_on (void)                                                    // 开启RTC供电。40ms后方可操作
{
    MCU_IO_PERIPHERAL_1 (HAL_BOARD_IIC_SCL_PORT, HAL_BOARD_IIC_SCL_PIN, MCU_IO_DIR_INPUT);  // UCB1SCL
    MCU_IO_PERIPHERAL_1 (HAL_BOARD_IIC_SDA_PORT, HAL_BOARD_IIC_SDA_PIN, MCU_IO_DIR_INPUT);  // UCB1SDA
    MCU_IO_OUTPUT (HAL_BOARD_RTC_PWR_PORT, HAL_BOARD_RTC_PWR_PIN, HAL_BOARD_RTC_PWR_ON);
}

void rtc_power_off (void)                                                   // 关闭RTC供电，RTC使用备用电容低功耗运行。
{
    MCU_IO_OUTPUT (HAL_BOARD_RTC_PWR_PORT, HAL_BOARD_RTC_PWR_PIN, HAL_BOARD_RTC_PWR_OFF);
}

uint16 rtc_valid_check (void)                                           // 有效性检查。需已上电40ms以上
{
    uint16 ret_value;

    if (RX6110_dummy_read() == FALSE)                                   // Dummy read
    {
        RTC_available = FALSE;
        return (HAL_BOARD_IIC_BUS_ERROR);                               // IIC总线错误
    }

// *************************    初始化IIC  ************************************

    EUSCI_B_I2C_disable(HAL_BOARD_IIC_BASE);                                // 关闭IIC

    EUSCI_B_I2C_masterInit(HAL_BOARD_IIC_BASE,
                            EUSCI_B_I2C_CLOCKSOURCE_SMCLK,                  // 时钟源 SMCLK
                            HAL_CPU_CLOCK_KHz,                              // SMCLK时钟频率
                            333,                                            // I2C速率：333K
                            0,                                              // 不考虑自动计数
                            EUSCI_B_I2C_NO_AUTO_STOP                        // 不自动发送停止位
                            );

    MCU_IO_PERIPHERAL_1 (HAL_BOARD_IIC_SCL_PORT, HAL_BOARD_IIC_SCL_PIN, MCU_IO_DIR_INPUT);  // UCB1SCL
    MCU_IO_PERIPHERAL_1 (HAL_BOARD_IIC_SDA_PORT, HAL_BOARD_IIC_SDA_PIN, MCU_IO_DIR_INPUT);  // UCB1SDA

    EUSCI_B_I2C_setSlaveAddress(HAL_BOARD_IIC_BASE, HAL_BOARD_RX6110_ADD);  // RX6110 地址

    EUSCI_B_I2C_enable(HAL_BOARD_IIC_BASE);                                 // 开启IIC

// *************************    读VLF位       ************************************
    uint8 _check;

    EUSCI_B_I2C_clearInterruptFlag (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_NAK_INTERRUPT); // 清NACK标志

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                           // 发送写入，自动发送START标志、寻址字节，不包括ACK。

    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1E);                        // 写入第一字节地址
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                                // 此时ACK已检测完毕，第一字节已写入移位寄存器并正在发送

    if (EUSCI_B_I2C_getInterruptStatus (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_NAK_INTERRUPT)) // 如寻址错误，发送取消
    {
        EUSCI_B_I2C_masterMultiByteSendStop (HAL_BOARD_IIC_BASE);               // 立即发送停止位
        while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE);

        RTC_available = FALSE;
        ret_value = HAL_BOARD_RX6110_NOT_EXIST;                                 // 芯片不存在
    }
    else
    {
        EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_MODE);
        EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 重启
        while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                                // 此时正在接收第一字节

        EUSCI_B_I2C_masterMultiByteReceiveStop (HAL_BOARD_IIC_BASE);            // 发送停止位
        while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE); // 读取 FlagReg
        _check = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);                 // 读取RXBUF

        if ((_check & 0x02) ==0)    ret_value = HAL_BOARD_RX6110_READY;         // RTC芯片正常
        else ret_value = HAL_BOARD_RX6110_VLF_FAIL;
        RTC_available = TRUE;
    }

    return (ret_value);
}




uint16 RX6110_dummy_read (void)                 // 芯片要求的时序，无实际意义，不必理会。返回FALSE代表总线错误
{
    P3SEL0 &= 0xF9;                             // 为追求效率，此处直接写寄存器，注意移植与兼容问题
    P3SEL1 &= 0xF9;
    P3DIR &= 0xF9;                              // 串行引脚置为输入

    NOP();
    if ((P3IN & 0x06) != 0x06)      return (FALSE);

    P3OUT &= ~BIT1;                             // SDA 置低    ____
    P3DIR |= BIT1;                              //                \________
    P3OUT &= ~BIT2;                             // SCL 置低    ________
    P3DIR |= BIT2;                              //                    \____

    uint8 i = 18;
    P3DIR &= ~BIT1;                             // 设定SDA为高，读操作。（dummy read不理会地址内容，随意给，也不考虑ACK）
    while (i > 0)                                                               // 循环 18 次
    {
        P3DIR &= ~BIT2;
        P3OUT &= ~BIT2;
        P3DIR |= BIT2;                          // SCL 脉冲
        i--;
    }

    P3OUT &= ~BIT1;                             // SDA 置低    ___           ____
    P3DIR |= BIT1;                              //             ___X_________/
    P3DIR &= ~BIT2;                             // SCL 释放           ___________
    NOP();                                      //             ______/
    P3DIR &= ~BIT1;                             // SDA 释放

    return (TRUE);
}



uint16 rtc_read (void)                                                  // 读取时钟
{
    if (!RTC_available)     return (FALSE);                                 // RTC芯片若无效，返回

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 发送写入，自动发送START标志、寻址字节，不包括ACK。
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x10);                    // 地址10
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                            // 此时ACK已检测完毕，第一字节已写入移位寄存器并正在发送
    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 重启
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                            // 此时正在接收第一字节

    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0));
    rtc.second = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);             // 秒
    BCD_to_num_8 (rtc.second);

    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0));
    rtc.minute = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);             // 分
    BCD_to_num_8 (rtc.minute);

    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0));
    rtc.hour = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);               // 时
    BCD_to_num_8 (rtc.hour);

    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0));
    rtc.week = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);               // 周
    BCD_to_num_8 (rtc.week);

    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0));
    rtc.day = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);                // 日
    BCD_to_num_8 (rtc.day);

    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0));
    rtc.month = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);              // 月
    BCD_to_num_8 (rtc.month);

    EUSCI_B_I2C_masterMultiByteReceiveStop (HAL_BOARD_IIC_BASE);            // 发送停止位
    while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE);
    rtc.year = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);               // 年
    BCD_to_num_8 (rtc.year);

    return (TRUE);
}


uint16 rtc_preset (void)                                                // 设置时钟
{
    rtc.second = 0;
    rtc.minute = 0;
    rtc.hour = 10;

    rtc.day = 1;
    rtc.month = 1;
    rtc.year = 20;
    rtc.week = 1;

    rtc.hour_H = 0;                     // 清空无用寄存器

    if (!RTC_available)     return (FALSE);                                 // RTC芯片若无效，返回

    uint8 _check;

    do
    {
        //  ----------------    清VLF位   ------------------------------
        EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
        EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                   // 发送写入，自动发送START标志、寻址字节，不包括ACK。
        EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1E);                // 地址1E
        while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                            // 此时ACK已检测完毕，地址已写入移位寄存器并正在发送
        EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                // VLF写0
        while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

        EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                   // 重启
        EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1E);                // 地址1E
        while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                            // 此时ACK已检测完毕，地址已写入移位寄存器并正在发送
        //  ----------------    读VLF位   ------------------------------
        EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_MODE);
        EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                   // 重启
        while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                            // 此时正在接收第一字节

        EUSCI_B_I2C_masterMultiByteReceiveStop (HAL_BOARD_IIC_BASE);        // 发送停止位
        while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE); // 读取 FlagReg
        _check = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);             // 读取RXBUF

    }while ((_check & 0x02) !=0);                                       // 检测VLF是否已写0


//  ************************    时钟已起振   ********************************************


    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 发送写入，自动发送START标志、寻址字节，不包括ACK。
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x17);                    // 写入地址17
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                            // 此时ACK已检测完毕，第一字节已写入移位寄存器并正在发送
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0xA8);                    // 保留17 必须写A8
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x80);                    // 定时：分 关闭
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x80);                    // 定时：时 关闭
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x80);                    // 定时：日 关闭
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 重启写入
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x30);
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕

    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                    // 保留30
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1C);                    // 保留31 电压监测：最省电
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x04);                    // 保留32 输出控制    周期中断：IRQ1       时钟输出: IRQ2
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));


    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 重启写入
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1B);
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕

    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                    // 定时器0
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                    // 定时器1
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x01);                    // Extension    时钟输出:OFF    分/秒关闭   周期关闭，单位时间15.625ms
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                    // Flag         清0所有标志位
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x40);                    // Control      STOP，           分/秒中断关闭 定时中断关闭  周期中断关闭，一直计时
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));


    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 重启写入
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x10);
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕

    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                    // 秒
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                    // 分
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                    // 时
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x01);                    // 周
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x01);                    // 日
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x01);                    // 月
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x20);                    // 年
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 重启写入
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1F);
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕

    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                    // RUN
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

    EUSCI_B_I2C_masterMultiByteSendStop (HAL_BOARD_IIC_BASE);               // 停止IIC
    while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE);

    return (TRUE);

}


uint16 rtc_alarm (uint32 _time)                                         // 设置唤醒周期。调用前需确保RTC已上电40ms以上。参数为毫秒，误差可达20ms。不超过1020秒，即1020000。
{                                                                       // 如需更长的时间或更精确的唤醒，需修改单位时间TSEL
    uint8 _exten;

    if (!RTC_available)     return (FALSE);                                 // RTC芯片若无效，返回

    _time = (_time*1000) /15625;                                            // 计算定时时间，单位时间15.625ms
    if (_time ==0)  _time =1;                                               // 不可为0秒

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 发送写入，自动发送START标志、寻址字节，不包括ACK。
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1D);                    // 写入地址1D
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                            // 此时ACK已检测完毕，第一字节已写入移位寄存器并正在发送

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 重启
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                                // 此时正在接收第一字节
    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 发送写入，自动发送START标志、寻址字节，不包括ACK。
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1D);                    // 地址1D
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                            // 此时ACK已检测完毕，地址已写入移位寄存器并正在发送
    _exten = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);                 // 读取RXBUF
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, (_exten &0xEF));          // Extension    周期关闭
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                    // Flag         清0所有标志位
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x10);                    // Control      使能TIE
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));


    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 重启写入
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x32);
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕

    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x04);                    // 保留32 输出控制    周期中断：IRQ1       时钟输出: IRQ2
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));


    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 重启写入
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1B);
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕

    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, LO_UINT16(_time));        // 定时器0
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, HI_UINT16(_time));        // 定时器1
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, (_exten |0x10));          // Extension    周期开启
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

    EUSCI_B_I2C_masterMultiByteSendStop (HAL_BOARD_IIC_BASE);               // 停止IIC
    while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE);

    return (TRUE);
}


uint16 rtc_alarm_off (void)                                             // 关闭唤醒。调用前需确保RTC已上电40ms以上。
{
    uint8 _exten;

    if (!RTC_available)     return (FALSE);                                 // RTC芯片若无效，返回

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 发送写入，自动发送START标志、寻址字节，不包括ACK。
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1D);                    // 写入地址1D
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                            // 此时ACK已检测完毕，第一字节已写入移位寄存器并正在发送

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 重启
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                                // 此时正在接收第一字节
    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 发送写入，自动发送START标志、寻址字节，不包括ACK。
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1D);                    // 地址1D
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                            // 此时ACK已检测完毕，地址已写入移位寄存器并正在发送
    _exten = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);                 // 读取RXBUF
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, (_exten &0xEF));          // Extension    周期关闭
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

    EUSCI_B_I2C_masterMultiByteReceiveStop (HAL_BOARD_IIC_BASE);        // 发送停止位
    while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE); // 读取 FlagReg

    return (TRUE);
}


uint16 rtc_sync (void)                                                  // 同步RTC时钟。调用前需确保RTC已上电40ms以上。执行本指令后，IRQ2立即回到低电位
{
    if (!RTC_available)     return (FALSE);                                 // RTC芯片若无效，返回

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 发送写入，自动发送START标志、寻址字节，不包括ACK。
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x10);                    // 写入地址10
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                            // 此时ACK已检测完毕，第一字节已写入移位寄存器并正在发送
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                    // 秒
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

    EUSCI_B_I2C_masterMultiByteSendStop (HAL_BOARD_IIC_BASE);               // 停止IIC
    while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE);

    return (TRUE);
}


uint16 rtc_sync_en (void)                                               // 在IRQ2上输出同步时钟。调用前需确保RTC已上电40ms以上
{
    uint8 _exten;

    if (!RTC_available)     return (FALSE);                                 // RTC芯片若无效，返回

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 发送写入，自动发送START标志、寻址字节，不包括ACK。
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1D);                    // 地址1D
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                            // 此时ACK已检测完毕，地址已写入移位寄存器并正在发送
    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 重启
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                                // 此时正在接收第一字节
    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 发送写入，自动发送START标志、寻址字节，不包括ACK。
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1D);                    // 地址1D
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                            // 此时ACK已检测完毕，地址已写入移位寄存器并正在发送
    _exten = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);                 // 读取RXBUF
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, (_exten |0x40));          // 时钟输出: 1Hz
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

    EUSCI_B_I2C_masterMultiByteReceiveStop (HAL_BOARD_IIC_BASE);        // 发送停止位
    while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE); // 读取 FlagReg

    return (TRUE);
}


uint16 rtc_sync_dis (void)                                              // 在IRQ2上关闭同步时钟。调用前需确保RTC已上电40ms以上
{
    uint8 _exten;

    if (!RTC_available)     return (FALSE);                                 // RTC芯片若无效，返回

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 发送写入，自动发送START标志、寻址字节，不包括ACK。
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1D);                    // 地址1D
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                            // 此时ACK已检测完毕，地址已写入移位寄存器并正在发送
    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 重启
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                                // 此时正在接收第一字节
    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // 发送写入，自动发送START标志、寻址字节，不包括ACK。
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1D);                    // 地址1D
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // 等待启动信号完毕
                                                                            // 此时ACK已检测完毕，地址已写入移位寄存器并正在发送
    _exten = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);                 // 读取RXBUF
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, (_exten &0x3F));          // 时钟输出: OFF
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

    EUSCI_B_I2C_masterMultiByteReceiveStop (HAL_BOARD_IIC_BASE);            // 发送停止位
    while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE);

    return (TRUE);
}




void rtc_run (void)
{
    rtc.second += 1;                                    // 每隔1秒，秒+1
    if (rtc.second >= 60)                               // 超过1分钟
    {   rtc.second -= 60;                               // 秒减60

        rtc.minute++;                                   // 分+1
        if (rtc.minute >=60)                            // 超过1小时
        {   rtc.minute -= 60;                           // 分清0

            rtc.hour++;                                 // 小时+1
            if (rtc.hour >=24)                          // 超过1天
            {   rtc.hour = 0;                           // 小时清0

                rtc.day++;                              // 日期+1
                if (rtc.day >=31)                       // 超过30天
                {   rtc.day = 1;                        // 日期归1

                    rtc.month++;                        // 月+1
                    if (rtc.month >12)                  // 超过12月
                    {   rtc.month = 1;                  // 月归1

                        rtc.year++;                     // 年+1
                    }
                }

                rtc.week++;                             // 星期+1
                if (rtc.week >7)    rtc.week = 1;
            }
        }
    }
}
