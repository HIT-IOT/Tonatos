#include <RTC.h>
#include <macros.h>
#include <msp430.h>
#include <i2c.h>
#include <hal.h>
RTCControl rtc;
static bool RTC_available = FALSE;                                      // RTC�Ƿ���Ч

void rtc_power_on (void)                                                    // ����RTC���硣40ms�󷽿ɲ���
{
    MCU_IO_PERIPHERAL_1 (HAL_BOARD_IIC_SCL_PORT, HAL_BOARD_IIC_SCL_PIN, MCU_IO_DIR_INPUT);  // UCB1SCL
    MCU_IO_PERIPHERAL_1 (HAL_BOARD_IIC_SDA_PORT, HAL_BOARD_IIC_SDA_PIN, MCU_IO_DIR_INPUT);  // UCB1SDA
    MCU_IO_OUTPUT (HAL_BOARD_RTC_PWR_PORT, HAL_BOARD_RTC_PWR_PIN, HAL_BOARD_RTC_PWR_ON);
}

void rtc_power_off (void)                                                   // �ر�RTC���磬RTCʹ�ñ��õ��ݵ͹������С�
{
    MCU_IO_OUTPUT (HAL_BOARD_RTC_PWR_PORT, HAL_BOARD_RTC_PWR_PIN, HAL_BOARD_RTC_PWR_OFF);
}

uint16 rtc_valid_check (void)                                           // ��Ч�Լ�顣�����ϵ�40ms����
{
    uint16 ret_value;

    if (RX6110_dummy_read() == FALSE)                                   // Dummy read
    {
        RTC_available = FALSE;
        return (HAL_BOARD_IIC_BUS_ERROR);                               // IIC���ߴ���
    }

// *************************    ��ʼ��IIC  ************************************

    EUSCI_B_I2C_disable(HAL_BOARD_IIC_BASE);                                // �ر�IIC

    EUSCI_B_I2C_masterInit(HAL_BOARD_IIC_BASE,
                            EUSCI_B_I2C_CLOCKSOURCE_SMCLK,                  // ʱ��Դ SMCLK
                            HAL_CPU_CLOCK_KHz,                              // SMCLKʱ��Ƶ��
                            333,                                            // I2C���ʣ�333K
                            0,                                              // �������Զ�����
                            EUSCI_B_I2C_NO_AUTO_STOP                        // ���Զ�����ֹͣλ
                            );

    MCU_IO_PERIPHERAL_1 (HAL_BOARD_IIC_SCL_PORT, HAL_BOARD_IIC_SCL_PIN, MCU_IO_DIR_INPUT);  // UCB1SCL
    MCU_IO_PERIPHERAL_1 (HAL_BOARD_IIC_SDA_PORT, HAL_BOARD_IIC_SDA_PIN, MCU_IO_DIR_INPUT);  // UCB1SDA

    EUSCI_B_I2C_setSlaveAddress(HAL_BOARD_IIC_BASE, HAL_BOARD_RX6110_ADD);  // RX6110 ��ַ

    EUSCI_B_I2C_enable(HAL_BOARD_IIC_BASE);                                 // ����IIC

// *************************    ��VLFλ       ************************************
    uint8 _check;

    EUSCI_B_I2C_clearInterruptFlag (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_NAK_INTERRUPT); // ��NACK��־

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                           // ����д�룬�Զ�����START��־��Ѱַ�ֽڣ�������ACK��

    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1E);                        // д���һ�ֽڵ�ַ
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                                // ��ʱACK�Ѽ����ϣ���һ�ֽ���д����λ�Ĵ��������ڷ���

    if (EUSCI_B_I2C_getInterruptStatus (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_NAK_INTERRUPT)) // ��Ѱַ���󣬷���ȡ��
    {
        EUSCI_B_I2C_masterMultiByteSendStop (HAL_BOARD_IIC_BASE);               // ��������ֹͣλ
        while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE);

        RTC_available = FALSE;
        ret_value = HAL_BOARD_RX6110_NOT_EXIST;                                 // оƬ������
    }
    else
    {
        EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_MODE);
        EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����
        while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                                // ��ʱ���ڽ��յ�һ�ֽ�

        EUSCI_B_I2C_masterMultiByteReceiveStop (HAL_BOARD_IIC_BASE);            // ����ֹͣλ
        while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE); // ��ȡ FlagReg
        _check = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);                 // ��ȡRXBUF

        if ((_check & 0x02) ==0)    ret_value = HAL_BOARD_RX6110_READY;         // RTCоƬ����
        else ret_value = HAL_BOARD_RX6110_VLF_FAIL;
        RTC_available = TRUE;
    }

    return (ret_value);
}




uint16 RX6110_dummy_read (void)                 // оƬҪ���ʱ����ʵ�����壬������ᡣ����FALSE�������ߴ���
{
    P3SEL0 &= 0xF9;                             // Ϊ׷��Ч�ʣ��˴�ֱ��д�Ĵ�����ע����ֲ���������
    P3SEL1 &= 0xF9;
    P3DIR &= 0xF9;                              // ����������Ϊ����

    NOP();
    if ((P3IN & 0x06) != 0x06)      return (FALSE);

    P3OUT &= ~BIT1;                             // SDA �õ�    ____
    P3DIR |= BIT1;                              //                \________
    P3OUT &= ~BIT2;                             // SCL �õ�    ________
    P3DIR |= BIT2;                              //                    \____

    uint8 i = 18;
    P3DIR &= ~BIT1;                             // �趨SDAΪ�ߣ�����������dummy read������ַ���ݣ��������Ҳ������ACK��
    while (i > 0)                                                               // ѭ�� 18 ��
    {
        P3DIR &= ~BIT2;
        P3OUT &= ~BIT2;
        P3DIR |= BIT2;                          // SCL ����
        i--;
    }

    P3OUT &= ~BIT1;                             // SDA �õ�    ___           ____
    P3DIR |= BIT1;                              //             ___X_________/
    P3DIR &= ~BIT2;                             // SCL �ͷ�           ___________
    NOP();                                      //             ______/
    P3DIR &= ~BIT1;                             // SDA �ͷ�

    return (TRUE);
}



uint16 rtc_read (void)                                                  // ��ȡʱ��
{
    if (!RTC_available)     return (FALSE);                                 // RTCоƬ����Ч������

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����д�룬�Զ�����START��־��Ѱַ�ֽڣ�������ACK��
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x10);                    // ��ַ10
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                            // ��ʱACK�Ѽ����ϣ���һ�ֽ���д����λ�Ĵ��������ڷ���
    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                            // ��ʱ���ڽ��յ�һ�ֽ�

    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0));
    rtc.second = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);             // ��
    BCD_to_num_8 (rtc.second);

    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0));
    rtc.minute = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);             // ��
    BCD_to_num_8 (rtc.minute);

    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0));
    rtc.hour = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);               // ʱ
    BCD_to_num_8 (rtc.hour);

    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0));
    rtc.week = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);               // ��
    BCD_to_num_8 (rtc.week);

    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0));
    rtc.day = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);                // ��
    BCD_to_num_8 (rtc.day);

    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0));
    rtc.month = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);              // ��
    BCD_to_num_8 (rtc.month);

    EUSCI_B_I2C_masterMultiByteReceiveStop (HAL_BOARD_IIC_BASE);            // ����ֹͣλ
    while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE);
    rtc.year = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);               // ��
    BCD_to_num_8 (rtc.year);

    return (TRUE);
}


uint16 rtc_preset (void)                                                // ����ʱ��
{
    rtc.second = 0;
    rtc.minute = 0;
    rtc.hour = 10;

    rtc.day = 1;
    rtc.month = 1;
    rtc.year = 20;
    rtc.week = 1;

    rtc.hour_H = 0;                     // ������üĴ���

    if (!RTC_available)     return (FALSE);                                 // RTCоƬ����Ч������

    uint8 _check;

    do
    {
        //  ----------------    ��VLFλ   ------------------------------
        EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
        EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                   // ����д�룬�Զ�����START��־��Ѱַ�ֽڣ�������ACK��
        EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1E);                // ��ַ1E
        while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                            // ��ʱACK�Ѽ����ϣ���ַ��д����λ�Ĵ��������ڷ���
        EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                // VLFд0
        while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

        EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                   // ����
        EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1E);                // ��ַ1E
        while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                            // ��ʱACK�Ѽ����ϣ���ַ��д����λ�Ĵ��������ڷ���
        //  ----------------    ��VLFλ   ------------------------------
        EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_MODE);
        EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                   // ����
        while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                            // ��ʱ���ڽ��յ�һ�ֽ�

        EUSCI_B_I2C_masterMultiByteReceiveStop (HAL_BOARD_IIC_BASE);        // ����ֹͣλ
        while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE); // ��ȡ FlagReg
        _check = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);             // ��ȡRXBUF

    }while ((_check & 0x02) !=0);                                       // ���VLF�Ƿ���д0


//  ************************    ʱ��������   ********************************************


    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����д�룬�Զ�����START��־��Ѱַ�ֽڣ�������ACK��
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x17);                    // д���ַ17
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                            // ��ʱACK�Ѽ����ϣ���һ�ֽ���д����λ�Ĵ��������ڷ���
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0xA8);                    // ����17 ����дA8
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x80);                    // ��ʱ���� �ر�
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x80);                    // ��ʱ��ʱ �ر�
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x80);                    // ��ʱ���� �ر�
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����д��
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x30);
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����

    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                    // ����30
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1C);                    // ����31 ��ѹ��⣺��ʡ��
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x04);                    // ����32 �������    �����жϣ�IRQ1       ʱ�����: IRQ2
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));


    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����д��
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1B);
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����

    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                    // ��ʱ��0
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                    // ��ʱ��1
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x01);                    // Extension    ʱ�����:OFF    ��/��ر�   ���ڹرգ���λʱ��15.625ms
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                    // Flag         ��0���б�־λ
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x40);                    // Control      STOP��           ��/���жϹر� ��ʱ�жϹر�  �����жϹرգ�һֱ��ʱ
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));


    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����д��
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x10);
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����

    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                    // ��
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                    // ��
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                    // ʱ
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x01);                    // ��
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x01);                    // ��
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x01);                    // ��
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x20);                    // ��
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����д��
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1F);
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����

    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                    // RUN
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

    EUSCI_B_I2C_masterMultiByteSendStop (HAL_BOARD_IIC_BASE);               // ֹͣIIC
    while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE);

    return (TRUE);

}


uint16 rtc_alarm (uint32 _time)                                         // ���û������ڡ�����ǰ��ȷ��RTC���ϵ�40ms���ϡ�����Ϊ���룬���ɴ�20ms��������1020�룬��1020000��
{                                                                       // ���������ʱ������ȷ�Ļ��ѣ����޸ĵ�λʱ��TSEL
    uint8 _exten;

    if (!RTC_available)     return (FALSE);                                 // RTCоƬ����Ч������

    _time = (_time*1000) /15625;                                            // ���㶨ʱʱ�䣬��λʱ��15.625ms
    if (_time ==0)  _time =1;                                               // ����Ϊ0��

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����д�룬�Զ�����START��־��Ѱַ�ֽڣ�������ACK��
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1D);                    // д���ַ1D
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                            // ��ʱACK�Ѽ����ϣ���һ�ֽ���д����λ�Ĵ��������ڷ���

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                                // ��ʱ���ڽ��յ�һ�ֽ�
    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����д�룬�Զ�����START��־��Ѱַ�ֽڣ�������ACK��
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1D);                    // ��ַ1D
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                            // ��ʱACK�Ѽ����ϣ���ַ��д����λ�Ĵ��������ڷ���
    _exten = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);                 // ��ȡRXBUF
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, (_exten &0xEF));          // Extension    ���ڹر�
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                    // Flag         ��0���б�־λ
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x10);                    // Control      ʹ��TIE
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));


    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����д��
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x32);
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����

    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x04);                    // ����32 �������    �����жϣ�IRQ1       ʱ�����: IRQ2
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));


    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����д��
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1B);
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����

    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, LO_UINT16(_time));        // ��ʱ��0
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, HI_UINT16(_time));        // ��ʱ��1
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, (_exten |0x10));          // Extension    ���ڿ���
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

    EUSCI_B_I2C_masterMultiByteSendStop (HAL_BOARD_IIC_BASE);               // ֹͣIIC
    while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE);

    return (TRUE);
}


uint16 rtc_alarm_off (void)                                             // �رջ��ѡ�����ǰ��ȷ��RTC���ϵ�40ms���ϡ�
{
    uint8 _exten;

    if (!RTC_available)     return (FALSE);                                 // RTCоƬ����Ч������

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����д�룬�Զ�����START��־��Ѱַ�ֽڣ�������ACK��
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1D);                    // д���ַ1D
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                            // ��ʱACK�Ѽ����ϣ���һ�ֽ���д����λ�Ĵ��������ڷ���

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                                // ��ʱ���ڽ��յ�һ�ֽ�
    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����д�룬�Զ�����START��־��Ѱַ�ֽڣ�������ACK��
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1D);                    // ��ַ1D
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                            // ��ʱACK�Ѽ����ϣ���ַ��д����λ�Ĵ��������ڷ���
    _exten = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);                 // ��ȡRXBUF
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, (_exten &0xEF));          // Extension    ���ڹر�
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

    EUSCI_B_I2C_masterMultiByteReceiveStop (HAL_BOARD_IIC_BASE);        // ����ֹͣλ
    while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE); // ��ȡ FlagReg

    return (TRUE);
}


uint16 rtc_sync (void)                                                  // ͬ��RTCʱ�ӡ�����ǰ��ȷ��RTC���ϵ�40ms���ϡ�ִ�б�ָ���IRQ2�����ص��͵�λ
{
    if (!RTC_available)     return (FALSE);                                 // RTCоƬ����Ч������

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����д�룬�Զ�����START��־��Ѱַ�ֽڣ�������ACK��
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x10);                    // д���ַ10
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                            // ��ʱACK�Ѽ����ϣ���һ�ֽ���д����λ�Ĵ��������ڷ���
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                    // ��
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

    EUSCI_B_I2C_masterMultiByteSendStop (HAL_BOARD_IIC_BASE);               // ֹͣIIC
    while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE);

    return (TRUE);
}


uint16 rtc_sync_en (void)                                               // ��IRQ2�����ͬ��ʱ�ӡ�����ǰ��ȷ��RTC���ϵ�40ms����
{
    uint8 _exten;

    if (!RTC_available)     return (FALSE);                                 // RTCоƬ����Ч������

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����д�룬�Զ�����START��־��Ѱַ�ֽڣ�������ACK��
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1D);                    // ��ַ1D
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                            // ��ʱACK�Ѽ����ϣ���ַ��д����λ�Ĵ��������ڷ���
    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                                // ��ʱ���ڽ��յ�һ�ֽ�
    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����д�룬�Զ�����START��־��Ѱַ�ֽڣ�������ACK��
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1D);                    // ��ַ1D
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                            // ��ʱACK�Ѽ����ϣ���ַ��д����λ�Ĵ��������ڷ���
    _exten = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);                 // ��ȡRXBUF
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, (_exten |0x40));          // ʱ�����: 1Hz
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

    EUSCI_B_I2C_masterMultiByteReceiveStop (HAL_BOARD_IIC_BASE);        // ����ֹͣλ
    while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE); // ��ȡ FlagReg

    return (TRUE);
}


uint16 rtc_sync_dis (void)                                              // ��IRQ2�Ϲر�ͬ��ʱ�ӡ�����ǰ��ȷ��RTC���ϵ�40ms����
{
    uint8 _exten;

    if (!RTC_available)     return (FALSE);                                 // RTCоƬ����Ч������

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����д�룬�Զ�����START��־��Ѱַ�ֽڣ�������ACK��
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1D);                    // ��ַ1D
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                            // ��ʱACK�Ѽ����ϣ���ַ��д����λ�Ĵ��������ڷ���
    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                                // ��ʱ���ڽ��յ�һ�ֽ�
    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����д�룬�Զ�����START��־��Ѱַ�ֽڣ�������ACK��
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x1D);                    // ��ַ1D
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                            // ��ʱACK�Ѽ����ϣ���ַ��д����λ�Ĵ��������ڷ���
    _exten = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);                 // ��ȡRXBUF
    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, (_exten &0x3F));          // ʱ�����: OFF
    while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

    EUSCI_B_I2C_masterMultiByteReceiveStop (HAL_BOARD_IIC_BASE);            // ����ֹͣλ
    while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE);

    return (TRUE);
}




void rtc_run (void)
{
    rtc.second += 1;                                    // ÿ��1�룬��+1
    if (rtc.second >= 60)                               // ����1����
    {   rtc.second -= 60;                               // ���60

        rtc.minute++;                                   // ��+1
        if (rtc.minute >=60)                            // ����1Сʱ
        {   rtc.minute -= 60;                           // ����0

            rtc.hour++;                                 // Сʱ+1
            if (rtc.hour >=24)                          // ����1��
            {   rtc.hour = 0;                           // Сʱ��0

                rtc.day++;                              // ����+1
                if (rtc.day >=31)                       // ����30��
                {   rtc.day = 1;                        // ���ڹ�1

                    rtc.month++;                        // ��+1
                    if (rtc.month >12)                  // ����12��
                    {   rtc.month = 1;                  // �¹�1

                        rtc.year++;                     // ��+1
                    }
                }

                rtc.week++;                             // ����+1
                if (rtc.week >7)    rtc.week = 1;
            }
        }
    }
}
