# RTC - ʵʱʱ��

## ����
ϵͳʵʱʱ�ӣ�δʹ��DMA��Ҳδʹ��IIC�жϣ�ʹ��IIC��RX6110ͨ�š�

ע��RX6110�ϵ�40ms�󷽿ɲ�������ȡVLF-bit����Ϊ1������ʱ�Ӷ�ʧ���ٵ�1����ʼ������Ϊ0����ֱ�Ӳ�����

��ģ��δ�����жϵ�Ӱ�졣�����жϳ�����ܻ���IICͨ��ʱ����RTCͨ�������⣬�ɿ��������жϡ�

RX6110������λ��MSP430��IICģ��ϸ��ӣ������г���ʹ��˵����

������MSP430оƬ��IICģ���нϴ�Ķ�����ʹ�ò������ᵼ��IICͨ��������SCL���͡�����ԭ��μ��ֲ�ͼ26-12��26-13��

## RX6110 ����
����16�ֽ�RAM���Զ��ı��ù������

�����ֹ���ģʽ��
1. VDD��ѹ����1.6VʱΪ����ģʽ��������ͨ�ţ�
2. VDD��ѹ����1.6VΪʱ��ά��ģʽ��ʹ�ñ��õ�Դ���磬ͨ�Ž�ֹ��IRQ2��ֹ

ʱ��ά����͵�ѹ��1.1V��  IICͨ����͵�ѹ��1.6V��     ������ѹ���ޣ�5.5V��    ������ܵ�ѹ��6.5V��
ʱ��ά��ģʽ������130nA

��һ���ϵ羧��������Ҫ0.3�룬�Ϊ1��
VDD�ϵ�����ٵȴ�40ms��ͨ�š�200ms����ݳ�����
�����32K/1K/1Hz��Ƶ�ʣ�����3V���32Kʱ���ĵ�������Ϊ1.5uA
��·�ж������IRQ1Ϊ��©�����һֱ��Ч��IRQ2ΪCMOS�����1.6V������Ч

��֧��SPIͨ�Ż�IICͨ��
IIC��֧��400K���ʣ���SCL�ĸߵ͵�ƽʱ�䲻�Գơ������Գƣ�������Ӧ����333K����
IIC��ַ��7λ��ַ��0x64

IEA�ڵ����ʱ�ѿ������Ӳ��Ҫ�󣬲���׸��

## RX6110 ����
1. ��ʱ��������ʱ���룬���ڡ�2000-2099�꣬�Զ����ꡣ��ȡʱ�Զ�����
2. IRQ1ר���жϣ��������ڶϵ绽�ѣ�
        ��ʱ�жϣ�ָ��������ʱ�֣�ע�����룩������������ƽ
        ÿ��/���жϣ����崥����ƽ����7.813ms
        1Hz�жϡ�ע�����Ϊ�ǶԳƷ���
4. IRQ1��IRQ2�����жϣ�
        �����жϣ�244.14 us ~ 65535 hours�����崥����ƽ
        ʱ�����
5. �������ܣ�
        �ۼ��ϵ�/�ϵ�ʱ��
        ʱ����Ч���



RX6110 REG��ʽ��
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

*0: ����ĸ�ʽ���ܵ���ʱ�Ӵ���
*1: д��ʱ���ڲ���ʱ���Զ�����
*2: ֻ��һλΪ1
*3: ���ڸ��������Զ���λ
*4: ����д��0xA8
*5: AE��Ϊ1������ԸüĴ�������AEȫΪ1����ÿ���Ӵ���һ�Ρ�


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

��ʱ�жϣ� 0x18~0x1A��WADA��AF��AIE
        ����ǰ���Ƚ�AIE��0��
        �������ʱ��ǡ���������Ӷ�ʱ��ʱ�����ӻ�ȵ��´�ƥ��ʱ�ٴ����жϣ���������������
    WADA: ѡ��0x1A����
        0��      WEEK���ɶ�ѡ
        1��      DAY
    AF��Ϊ1���������жϡ����ֶ���0��ʹIRQ1�ָ���
    AIE����1ʹ��IRQ1���

�����жϣ� 0x1B��0x1C��TE��TF��TIE��TSEL2��TSEL1��TSEL0��TBKON��TBKE��TMPIN
        ����ǰ���Ƚ�TE��0
    ��0x1B��0x1C��Ԥ���ֵ��ʼ����0�������жϲ���ʼ���ּ�ʱ��
    0x1B+0x1C����λ��������Χ1-65535����TEΪ1ʱ��������˲ʱֵ
    TSEL2+TSEL1+TSEL0����λʱ���� *����һ���������ڴﲻ����λʱ�䡣               ************
        000:    ÿ��λ244.14us���15.99�롣       �жϳ���122us
        001:    ÿ��λ15.625ms���1023.9�롣  �жϳ���7.813ms                     ************
        010:    ÿ��λ1sec��    �18.2Сʱ��   �жϳ���7.813ms
        011:    ÿ��λ1min��    �45.5�졣    �жϳ���7.813ms
        100:    ÿ��λ1hour��   �7.48�ꡣ    �жϳ���7.813ms
    TE����1ʹ�������жϣ������¿�ʼ��ʱ
    TF��Ϊ1�����������жϡ����ֶ���0
    TIE����1ʹ��IRQ���
    TBKON+TBKE��
        00�� һֱ��ʱ
        01�� �ϵ�ʱ��ʱ
        10�� һֱ��ʱ
        11�� ��ع���ʱ��ʱ
    TMPIN:
        0��      �����IRQ2
        1��      �����IRQ1

ÿ��/���жϣ� USEL��UF��UIE
        ����ǰ���Ƚ�UIE��0
    USEL: 0Ϊ�룬1Ϊ��
    UF��Ϊ1�����������жϡ����ֶ���0
    UIE����1ʹ��IRQ1���

1Hz�жϣ�
    TMREC����1ʹ��1Hz�����IRQ1

ʱ������� FSEL1��FSEL0��FOPIN0
        ֻ�г�IEA�ڵ����õ�����
    FOPIN0��0�����IRQ2��1�����IRQ1�����ʱ����Ч��
    FSEL1+FSEL0��
        00�� OFF (d)
        01�� 1Hz
        10�� 1024Hz
        11�� 32768Hz(��Ҫ�����IRQ2)

ʱ����Ч��⣺ VLF
    ��VLFΪ1ʱ��˵��ʱ���Ѿ���ʧ����Ҫ���³�ʼ��

ʱ��ֹͣ�� STOP
    STOPΪ1ʱ��ʱ��ֹͣ��ʱ������ж�ֹͣ�������ж�ֹͣ��
        32K��1Kʱ��������֣�1Hzʱ��ֹͣ

��ѹ����BK SON��BK SOFF��BK SMP1��BK SMP0
    BK SON + BK SOFF��
        00�� ʹ�ñ��õ��
        01�� ֱ�ӹ��磬�ޱ��õ�Դ
        10�� ������ѹ�����Զ��л�����ģʽ����ѹ�豣����1.6V����
        11�� ʹ�ñ��õ�أ�ʡ��ģʽ
    BKSON + BKSOFF + BKSMP1 + BKSMP0��
        0000��   ��������ģʽһֱ����ѹ    ʱ��ά��ģʽÿ����2ms       ���ر�2ms����998ms�����2�룩
        0001��   ��������ģʽһֱ����ѹ    ʱ��ά��ģʽÿ����2ms       ���ر�16ms
        0010��   ��������ģʽһֱ����ѹ    ʱ��ά��ģʽÿ����2ms       ���ر�128ms
        0011��   ��������ģʽһֱ����ѹ    ʱ��ά��ģʽÿ����2ms       ���ر�256ms����ֻ����2ms��⣬�������254msҲû���õ�س�磩
        0100��   ��������ģʽÿ����2ms       ʱ��ά��ģʽÿ����2ms       �������õ�س��
        0101��   ��������ģʽÿ����16ms  ʱ��ά��ģʽÿ����2ms       �������õ�س��
        0110��   ��������ģʽÿ����128ms ʱ��ά��ģʽÿ����2ms       �������õ�س��
        0111��   ��������ģʽÿ����256ms ʱ��ά��ģʽÿ����2ms       �������õ�س��
        10xx��   ��������ģʽ��������ѹ    ʱ��ά��ģʽ��������ѹ    ���õ�س��һֱ����
        1100��   ��������ģʽÿ����2ms       ʱ��ά��ģʽÿ����2ms       ���ر�2ms����������ʡ�磩
        1101��   ��������ģʽÿ����16ms  ʱ��ά��ģʽÿ����2ms       ���ر�16ms
        1110��   ��������ģʽÿ����128ms ʱ��ά��ģʽÿ����2ms       ���ر�128ms
        1111��   ��������ģʽÿ����256ms ʱ��ά��ģʽÿ����2ms       ���ر�256ms





ע�⣺
MSP430оƬ��IICģ���нϴ�Ķ������������IICЧ�ʣ������Ӷ�Ҳ����ˡ�
���ԣ� 7/10λ��ַ��֧�ֹ㲥��֧�ֶ�������֧��4���ӵ�ַ��֧��DMA��֧��LPM4���ѣ�֧���Զ�������ֹͣ��
�Ķ��� ������λ��ֹͣλ�뷢��/���հ󶨣����ɵ���������ʹ��ǰһ��ҪŪ�������ԭ���μ��ֲ�ͼ26-12��26-13�������ʹ�ûᵼ�����߿���������
        ACK���ٵ�����⣬���뷢�Ͱ󶨣�����


ע�⣺
FR59xx�Ŀ�eusci_b_i2c��ʹ�ò������ᵼ��IICͨ��������SCL���͡�
���IICͨ�����ʹ�����»���ָ�

uint16_t EUSCI_B_I2C_isBusBusy(uint16_t baseAddress)                // �ж������Ƿ���У�˫���Ƿ�����
//!         - \b EUSCI_B_I2C_BUS_BUSY
//!         - \b EUSCI_B_I2C_BUS_NOT_BUSY

uint16_t EUSCI_B_I2C_masterIsStopSent(uint16_t baseAddress)         // �ж�ֹͣλ�Ƿ񷢳�����Ҫ��ִ��EUSCI_B_I2C_masterMultiByteReceiveStop
//!         - \b EUSCI_B_I2C_STOP_SEND_COMPLETE
//!         - \b EUSCI_B_I2C_SENDING_STOP

uint16_t EUSCI_B_I2C_masterIsStartSent(uint16_t baseAddress)        // �ж���ʼλ�Ƿ񷢳�����Ҫ��ִ��EUSCI_B_I2C_masterSendStart
//!         - \b EUSCI_B_I2C_START_SEND_COMPLETE
//!         - \b EUSCI_B_I2C_SENDING_START



void EUSCI_B_I2C_setMode(uint16_t baseAddress, uint8_t mode)        // ���÷���/���ա�����EUSCI_B_I2C_masterSendStartǰ����
//!        - \b EUSCI_B_I2C_TRANSMIT_MODE [Default]
//!        - \b EUSCI_B_I2C_RECEIVE_MODE

void EUSCI_B_I2C_masterSendStart(uint16_t baseAddress)              // ������ʼλ
                                                                    // ��Ϊ��������ģʽ������TXBUF����(����������)���Զ�����START��־��Ѱַ�ֽڣ�������ACK����TXBUFд����һ�������ֽ�ʱ�ż��ACK�������ACK��STT���㣬TXIFG��1��
                                                                    // ��Ϊ��������ģʽ���Զ�����START��־��Ѱַ�ֽڣ�����ACK��ACK������STT���㡣����ACK��NACKIFG��1��
                                                                            // ��ACK����������Զ����յ�һ���ֽڣ�������Ϻ�RXIFG��1
                                                                            // ����ACK��ȡ�����գ�NACKIFG��1����ʱ����ֹͣλ��ֱ�ӷ���ֹͣ����ʱ��������λ��ֱ�ӷ���START��־��Ѱַ�ֽڡ�

void EUSCI_B_I2C_masterMultiByteReceiveStop(uint16_t baseAddress)   // ����ֹͣλ
                                                                    // ��Ϊ��������ģʽ�������ڷ��ͣ����ڵ�ǰ�ֽڷ�����Ϻ���ֹͣλ�������ڿ��У�������һ�ֽڷ�����Ϻ���ֹͣλ������һ�ֽڵ�ACK��������������ֹͣλ
                                                                    // ��Ϊ��������ģʽ�����ڽ����У����ڵ�ǰ�ֽڽ�����Ϻ���NACK��������ֹͣλ��

void EUSCI_B_I2C_slaveDataPut(uint16_t baseAddress, uint8_t transmitData)   // ����һ���ֽ�
                                                                    // �����÷���ģʽ�����ȼ����һ���ֽڵ�ACK��
                                                                            // ��δͨ������ȡ�����ͣ�NACKIFG��1����ʱ����ֹͣλ��ֱ�ӷ���ֹͣ����ʱ��������λ��ֱ�ӷ���START��־��Ѱַ�ֽڡ�
                                                                            // ��ͨ����TXIFG��1������д���¸����ݣ���ʱ���ڷ����У�ֻ��TXBUFΪ�ա���ʱ����ֹͣλ������һ�ֽڷ�����Ϻ��͡�
                                                                            // �ڷ����ֽ�ʱ����������λ�����ڱ��ֽڷ�����Ϻ��Զ����ACK��������START��־��Ѱַ�ֽڡ�

uint8_t EUSCI_B_I2C_slaveDataGet(uint16_t baseAddress)              // ����һ���ֽڡ�
                                                                    // �����ý���ģʽ������⵽RXIFGΪ1ʱ���ã���ȡRXBUF���ݡ�
                                                                            // �ڽ����ֽ�ʱ����������λ�����ڱ��ֽڷ�����Ϻ��Զ�����NACK��������START��־��Ѱַ�ֽڡ�


void EUSCI_B_I2C_clearInterruptFlag(uint16_t baseAddress, uint16_t mask)        // ���־λ
uint16_t EUSCI_B_I2C_getInterruptStatus(uint16_t baseAddress, uint16_t mask)    // ����־λ
//!        - \b EUSCI_B_I2C_NAK_INTERRUPT - Not-acknowledge interrupt
//!        - \b EUSCI_B_I2C_STOP_INTERRUPT - STOP condition interrupt
//!        - \b EUSCI_B_I2C_START_INTERRUPT - START condition interrupt
//!        - \b EUSCI_B_I2C_TRANSMIT_INTERRUPT0 - Transmit interrupt0
//!        - \b EUSCI_B_I2C_RECEIVE_INTERRUPT0 - Receive interrupt0



��õȴ�ÿ��������ɺ���ִ�к���Ĳ���������IICʱһ����ʾ�����۲첨���Ƿ���ȷ������
���̣�д�룺

    EUSCI_B_I2C_clearInterruptFlag (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_NAK_INTERRUPT); // ��NACK��־

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                           // ����д�룬�Զ�����START��־��Ѱַ�ֽڣ�������ACK��

    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                        // д���һ�ֽڣ���ַ00
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                                // ��ʱACK�Ѽ����ϣ���һ�ֽ���д����λ�Ĵ��������ڷ���

    if (EUSCI_B_I2C_getInterruptStatus (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_NAK_INTERRUPT)) // ��Ѱַ����оƬ�����ڣ�����ȡ��
    {
        EUSCI_B_I2C_masterMultiByteSendStop (HAL_BOARD_IIC_BASE);               // ��������ֹͣλ
        while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE);
    }
    else
    {
        EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x01);                    // д1
        while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

        EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x02);                    // д2
        while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

        EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x03);                    // д3
        while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

        EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
        EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����
        EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x0F);                    // д��ڶ���ַ0F
        while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����

        EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x04);                    // д4
        while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

        EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x05);                    // д5
        while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

        EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x06);                    // д6
        while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0));

        EUSCI_B_I2C_masterMultiByteReceiveStop (HAL_BOARD_IIC_BASE);            // ��ʱ6���ڷ����У�����ֹͣλ��ע������м�����жϣ����ܻᵼ��ֹͣλ������ȥ������
        while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE); // ��ȡ FlagReg
    }


���̣���ȡһ�ֽڣ�

    EUSCI_B_I2C_clearInterruptFlag (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_NAK_INTERRUPT); // ��NACK��־

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                           // ����д�룬�Զ�����START��־��Ѱַ�ֽڣ�������ACK��

    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                        // д���һ�ֽڣ���ַ00
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                                // ��ʱACK�Ѽ����ϣ���һ�ֽ���д����λ�Ĵ��������ڷ���

    if (EUSCI_B_I2C_getInterruptStatus (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_NAK_INTERRUPT)) // ��Ѱַ����оƬ�����ڣ�����ȡ��
    {
        EUSCI_B_I2C_masterMultiByteSendStop (HAL_BOARD_IIC_BASE);               // ��������ֹͣλ
        while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE);
    }
    else
    {
        EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_MODE);
        EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����
        while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                                // ��ʱ���ڽ��յ�һ�ֽ�

        EUSCI_B_I2C_masterMultiByteReceiveStop (HAL_BOARD_IIC_BASE);            // ����ֹͣλ
        while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE); // ��ȡ FlagReg
        data = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);                   // ��ȡRXBUF
    }


���̣���ȡ���ֽڣ�

    EUSCI_B_I2C_clearInterruptFlag (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_NAK_INTERRUPT); // ��NACK��־

    EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                           // ����д�룬�Զ�����START��־��Ѱַ�ֽڣ�������ACK��

    EUSCI_B_I2C_slaveDataPut (HAL_BOARD_IIC_BASE, 0x00);                        // д���һ�ֽڣ���ַ00
    while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                                // ��ʱACK�Ѽ����ϣ���һ�ֽ���д����λ�Ĵ��������ڷ���

    if (EUSCI_B_I2C_getInterruptStatus (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_NAK_INTERRUPT)) // ��Ѱַ����оƬ�����ڣ�����ȡ��
    {
        EUSCI_B_I2C_masterMultiByteSendStop (HAL_BOARD_IIC_BASE);               // ��������ֹͣλ
        while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE);
    }
    else
    {
        EUSCI_B_I2C_setMode (HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_MODE);
        EUSCI_B_I2C_masterSendStart (HAL_BOARD_IIC_BASE);                       // ����
        while (EUSCI_B_I2C_masterIsStartSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_START_SEND_COMPLETE);   // �ȴ������ź����
                                                                                // ��ʱ���ڽ��յ�һ�ֽ�

        while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0));    // �ȴ�����1
        data1 = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);

        while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0));    // �ȴ�����2
        data2 = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);

        while (!EUSCI_B_I2C_getInterruptStatus(HAL_BOARD_IIC_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0));    // �ȴ�����3
        data3 = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);

        EUSCI_B_I2C_masterMultiByteReceiveStop (HAL_BOARD_IIC_BASE);            // ����ֹͣλ
        while (EUSCI_B_I2C_masterIsStopSent(HAL_BOARD_IIC_BASE) != EUSCI_B_I2C_STOP_SEND_COMPLETE); // ��ȡ FlagReg
        data4 = EUSCI_B_I2C_slaveDataGet (HAL_BOARD_IIC_BASE);                  // ��ȡ����4
    }


�ӿڡ�����˵����
void rtc_power_on (void)��RTC�ϵ硣�ϵ�40ms�󷽿�ͨ��
void rtc_power_off (void)���ر�RTC���磬RTCʹ�ñ��õ��ݵ͹������С�

���³������ǰ��ȷ��RTC���ϵ�40ms���ϡ�

uint16 rtc_valid_check (void)��RTC��Ч�Ժ�״̬���
        �ڵ��ϵ��Ҫͨ���ó����֪RTCģ���Ƿ񻹱�����ʱ�ӡ�
            ��ʱ�ӱ��֣������ֱ�Ӷ�ȡ��ͬ����
            ��ʱ�Ӷ�ʧ������Ҫͨ����������ͬ��RTC
void rtc_read (void)����ȡ������ʱ���룬Ӧ���ò�����
uint16 rtc_preset (void)����ʼ��RX6110оƬ�������ڲ��Ĵ�����������
uint16 rtc_wake (uint32 _time)�����û���ʱ�䡣����Ϊ���룬ʵ��ʱ�������0~15ms��_time��Ӧ����1020�룬��1020000��
            ������100000������100����ѽڵ㡣
                ���ʱ�ڵ�ϵ磬���Ѳ����У�C2��Ӧ���㹻�ĵ�����
                ���ʱ�ڵ������У�
                    ���ʱ�ڵ�C1��ѹ����2.7V����Ӱ���������
                    ���ʱ�ڵ�C1��ѹ����2.7V����C1��ѹ����2.7V��������Ӱ���������
            ע��ù����粻�رգ��������Ի��ѡ�
uint16 rtc_wake_off (void)���رջ���
uint16 rtc_sync (void)��ͬ��RTCʱ�ӡ�ִ�б�ָ���IRQ2�����ص��͵�λ�����¼�ʱ
uint16 rtc_sync_en (void)����IRQ2�����ͬ��ʱ�ӡ�
uint16 rtc_sync_dis (void)����IRQ2�Ϲر�ͬ��ʱ�ӡ�

void rtc_run (void)����ʱ��ÿ1�������1��
