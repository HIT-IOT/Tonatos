
/*
���ܣ�SX1211 ����оƬ ���������ģ�� for MSP430
���ߣ�����
������ڣ�2017.1.9

�������ڣ�2017.9.22
���Ĺ��ܣ������ĵ�����BUG

--------------------------------------------------------------------

������
������ΪSX1211 ����оƬ ���������ģ�� for MSP430����������ṩӲ���ӿڡ�
������Ҳ��ʹ����SX1212��

SX1211������863 - 960MHz��SX1212������300-510MHz

SX1211����ص㣺���յ���3mA

SX1211������
    ���ֹ���ģʽ�����ߡ����������������ա�����
    ��������ģʽ��IO���ֽڡ����ݰ�
            IOģʽ��ʱ��+��������SPI
            �ֽ�ģʽ�����ݰ��ֽڴ��ͣ�ʹ�û���
            ���ݰ�ģʽ��ʹ�����������ݰ��������ݣ�������ǰ����ͬ���֡���ַ�����ȡ��غɡ�CRC�����ݰ��Զ�����
    3mA�Ľ��յ�������������25mA�����ͨ������200Kb/s
    ����ģʽΪFSK��OOK�����͹���-8dBm - +13dBm��������ѹ2.1-3.6V
    ����64�ֽڵ�FIFO��ֻ�ܴ��ڷ���/����״̬���յ����ݰ�������ȡ���淽�ɽ��պ������ݰ�



ע�⣺
��ģ��δ�����ز�����/���з��ͣ�����ʱ���ܵ������߳�ͻ������RXģʽ��ͨ��SX1211_GET_RSSI ()��õ�ǰ�ź�ǿ�ȣ����ж������Ƿ�ռ�á�
ǿ���Ƽ�ͨ��������ɳ�ʼ����������������SX1211_PreSetCfg[][]�С�
�����������Ƶ�����Բ��ɸ��ģ���Ҫͨ��������ɣ��������������顣(��ͨ��Ƶ�ʡ����ʵ�)
Ϊ�˽��͹��ģ���СCPUʱ�䣬��������õ�����ȴ���ʽ��

������������Ҫ������FSKģʽͨ�ţ���OOK���л�����֧�֣���Щ�������ɸ��ġ�
��������õ�SX1211_Board.c��SMCLK��Ҫ�󣬼�˵��
�������ʼ��ʹ����TA2
��ģ��ܶ��ӳ���������ض��Ļ���������������ݰ�ģʽ��������OOK�ȣ�����������δ���ʹ�û����Ƿ��ʺϣ������в���˵����ע��
������/��ʱ����SPIͨ�ſ��ܻ�Ӱ�������ȡ�

�ز�Ƶ�ʼ�ͨ������ȡ��������ģ���Ӳ����ƣ�ʹ��ǰӦ�Ȳ���ģ���Ƶ�����ԣ�ѡ����ʵ�ͨ�Ų�����
    ����Ӧƥ��ģ���Ƶ�����ԡ�������ģ�������ߣ�
�źŵ����������ߵ����͡�λ�á��ڷ��кܴ��ϵ��ͨ���쳣ʱ�ɳ������°ڷ����ߡ�
    ��ʵ�⣬���弰��Χ�������ź�����û�б����ϵ�Ӱ�졣
    ����ʱ�ϸ�����¶�ʱ�䣬����ʱ���������ģ�顣
������ѹ���ó���3.6V��ʹ��ʱ���ϸ�ȷ������ƽ�ȣ����򼫿��ܳ��ָ����쳣����������
SX1211���ܻ���������ʼ����Ч����ʱӦ�ϵ�����

�ɱ䳤�����ݰ�ģʽʱ���������ݰ��ĵ�һ���ֽڱ���Ϊ�����ֽڣ��Ҳ���Ϊ0��                                                                        ������
�����յ�������ݶ�MCUδ��Ӧʱ���ᵼ��RX FIFO��������ٲ��������жϡ�                                                                     ������
    ��MCU״̬��ȷ��ʱ������CCоƬ״̬(RF_Mode_Status)��ȷ���䴦����������״̬��                                                           ������
    ��оƬ״̬�쳣ʱ���ɵ���RF_Mode_RX ���½������ģʽ��                                                                                    ������

���ݰ�ģʽ�£�ĳЩͬ���֣���0xAAAA���ᵼ��ͨ��ʧЧ����Ҫ��������ֵ����                                                                      ������
���ݰ�ģʽ��PktParam1�е�ֵΪRXʱ�����յ����ݳ��ȣ����ù�С�ᵼ���޷����գ���
δ���Ƕ�֡��������������Ӧ������64�ֽ�����
�翪��CRC��У��ʧ�ܵİ�ֱ���ӵ�������
����FIFO����ʱ��Ӧ�յ�EMPTYΪֹ
    SX1211_FIFO_Standby (SX1211_STDBY_FIFO_READ);           // ʹ�ý��յ����ݺ󣬽���STDBYģʽҲ������
���� SX1211_Board �еĺ���ǰ��һ��ȷ�� SX1211_SPI_BUSY Ϊ�٣�����ԭ��ͨ������ֱ�ӱ���ϲ�ȡ��
��ģ����δ���ǿ��Ź�



SX1211/SX1212ͨ��˵����
    �շ�����ʹ����ͬоƬ��Ƶ�Ρ�
����ģʽ��
    ��Ҫ��˯�ߡ����������������͡���������ģʽ��
    ˯�ߣ�оƬֹͣ��������͵���3uA��������Ҫʱ�䡣
    ������оƬ�������ɶԼĴ���/���������ƽ������1.7mA��
    ������VCO��������ʱ׼���շ���ƽ������1.7mA��
    ���ͣ���FIFO�е����ݷ��ͳ�ȥ��˲ʱ�ĵ�Լ17mA/1dbm��
    ���գ��������״̬�������ĵ�Լ17mA��

����ģʽ��
    ��ģ��֧�����ֵ���ģʽ��FSK/OOK��ͨ��SX1211_Modulation_Select()ѡ��

��ַ��
    �����޵�ַУ�飬��ͬƵ��ͨ������ͨ�ţ������ַУ���ֻ����ͬ��ַ��㲥���ݲſɽ��ա�����ο� SX1211_Address_FILT()   SX1211_Address()
    ע�������ַƥ��ʧ�ܣ����������ݰ�����������������ģʽ��                                                                               ������

���ȣ�
    ֧���������ݳ��ȣ�ͨ������ SX1211_Length_Set()   SX1211_Length()��ѡ�����ݳ���
        �̶����ݰ�����ģʽ�����ݰ������� PktParam1 �Ĵ��������á����������ݰ���������ա�                                                  ������
        �ɱ����ݰ�����ģʽ��ͨ��ͬ���ֺ���ĵ�һ���ֽ����������ݰ����ȣ��������������ֽ�����
                PktParam1 �Ĵ�����������RXģʽ�������������ݰ����ȡ��κγ����ֽ�ֵ���� PktParam1 �Ľ������ݰ�����������
    ��ģ���趨Ϊ�ɱ����ݰ�����ģʽ�����ݳ�����С�ڵ���64(�������ֽ�)��
    ������ģ�齫����������һ��д�뻺���ͳһ���͡�
    ��ģ����δ����IOģʽ���ֽ�ģʽ��



����״̬ͼ��
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
ʵ��������״̬���������л�����ʵ�ʹ�������ͼ��ʾ��




ϸ��˵����

CLKOUT���ϵ���Զ���������⵽CLKOUT˵��оƬ���ڲ��Ѿ�����
    ���źŹ��ı�430�Դ��ľ���󣬿ɹر��Խ��ܡ��������л���ʱ���źű���������ǰ��״̬��
    ������Ϊ���оƬ�Ƿ���������ź�Ӧһֱ������

IF2�Ƽ�Ϊ100KHz
FSKʱ��FdevΪ����Ƶ�ʣ�Fdev����33K
OOKʱ��Fdev����100K
�������ü���37��38

RSSI ����3db��ÿdev���ڲ�16�Σ���Ͳȡ��ֵ�������ֵÿdev���ڸ���16��


������δ���ǵĹ����У�
    �������ݷ���
    IOģʽ
    �ֽ�ģʽ
    ASKģʽ

�ɹ����Ի�����MCU��Ƶ4M��MSP430FR6979��IAR EW430 6300
��ģ��һ��ֻ�շ�һ��������˫��ͨ��



�ӿڡ�����˵����
    dΪ�ϵ�Ĭ��ֵ���Ǳ�ģ���ʼ��Ĭ��ֵ

uint8 initiate_SX1211 (void)����ʼ��ģ�顣
    ���ӳ����ʼ��SPI����ʼ��оƬ�ڲ��Ĵ��������ù���ģʽ��
    ����������Ϣ�������üĴ���SX1211_PreSetCfg[][] ����˵����ǿ���Ƽ�ͨ��������ɳ�ʼ������



PLL����ʱ��Լ300uS

�������


*/

#include <SX1211.h>

#include <hal.h>
#include <timer_a.h>
#include <macros.h>
#include <MAC_radio.h>

/*
��Ƶ���ԣ�           �����37
    ��һƵ�Σ�               868M
    �ڶ�Ƶ�Σ�               869M
    �������ʣ�               100K        Ĭ��
    λ���ʣ�                25K         Ĭ��
    ʵ�ʴ������ʣ�         3KB/s
    ʵ�����ݰ��������ʣ�  330 - 2.7K Byte/s
    ǰ���֣�                25C4 H

Ԥ�ò�����
    ����ģʽ��               ����
    ����ģʽ��               ���ݰ�
    ����ģʽ��               FSK
    FIFO�ߴ磺             64�ֽ�
    ���͹��ʣ�               -8dBm
    ǰ��/ͬ���֣�         3�ֽ�/2�ֽ�
    ��ַУ�飺               �ر�
    ���ݳ��ȣ�               �䳤
    CRCУ�飺              ����
    CRC�Զ������            ����
    ����˹�ر��룺         �ر�
    ���ݰ׻���               �ر�
    CLKOUT�����           ������200K
    PLL_LOCK�����         ����
    ͬ���֣�                25C4 H
    IRQ0��                   �ӵ���
    IRQ1��                   ����CRC_OK�������
    ���ͣ�                 ����FIFO��ֵ����
    ����ģʽFIFO��           д��


Ĭ�����ã�
    �ֽ�0��    McParam1        ����ģʽ��868M��Χ��VCOĬ�ϣ���һ��Ƶ��
    �ֽ�1��    McParam2        FSK�����ݰ�ģʽ��OOK��ֵ���Ŵ�����0dB
    �ֽ�2��    Fdev            �����ʣ�/�������           100K        �����37�����ɵ���33K
    �ֽ�3��    Bitrate         ���ݴ����ʣ�/�������     25K         �����37
    �ֽ�4��    OokFloorThresh  OOK������ֵ����
    �ֽ�5��    McParam6        FIFO��FIFO��ֵ             64�ֽ�/1�ֽ�
    �ֽ�6��    R1              R1��/�������
    �ֽ�7��    P1              P1��/�������
    �ֽ�8��    S1              S1��/�������                868M
    �ֽ�9��    R2              R2��/�������
    �ֽ�A��    P2              P2��/�������
    �ֽ�B��    S2              S2��/�������                869M
    �ֽ�C��    PaRamp          OOK��ƽʱ��                 23uS

    �ֽ�D��    IrqParam1       IRQ0�յ�����IRQ1����ʱCRCOK        ����ʱ���       ����������
    �ֽ�E��    IrqParam2       FIFO�Զ���䣬������ֵSX1211_FIFO_THRESH���ͣ�PLL������������RSSI_IRQ��־
    �ֽ�F��    RssiIrqThresh   RSSI�ж���ֵ

    �ֽ�10��RxParam1       �˲������ã���3.4.4
    �ֽ�11��RxParam2       �˲�������
    �ֽ�12��RxParam3       ����λͬ��������ͬ���֣�ͬ����2�ֽڣ��ݴ�1λ
    �ֽ�13��Res19          ����
    �ֽ�14��RssiValue      RSSI��ֻ����
    �ֽ�15��RxParam6       OOK�����ֵ����3.4.10.2

    �ֽ�16��SyncByte1      ͬ����1
    �ֽ�17��SyncByte2      ͬ����2
    �ֽ�18��SyncByte3      ͬ����3
    �ֽ�19��SyncByte4      ͬ����4

    �ֽ�1A��TxParam            ���͹��� -8dB
    �ֽ�1B��OscParam       CLKOUT������Լ200K
    �ֽ�1C��PktParam1      ������˹�أ��غɳ���3F
    �ֽ�1D��NodeAddr       ������ַ
    �ֽ�1E��PktParam3      �䳤���ݣ�ǰ��3�ֽڣ��رհ׻�������CRC���޵�ַУ��
    �ֽ�1F��PktParam4      CRC�����Զ����������Ĭ��д
*/


static uint8 SX1211_PreSetCfg[32][2] = {        // Ԥ��SX1211������
    {0x00, 0x10},       // McParam1         ģʽ1
    {0x02, 0x8C},       // McParam2         ģʽ2
    {0x04, 0x03},       // Fdev             ����Ƶ��    �Զ�����        Ĭ��ֵ
    {0x06, 0x07},       // Bitrate          ��������    �Զ�����        Ĭ��ֵ
    {0x08, 0x0C},       // OokFloorThresh   OOK����       �Զ�����        Ĭ��ֵ
    {0x0A, 0xC1},       // McParam6         FIFO����
    {0x0C, 0x7D},       // R1                           �Զ�����
    {0x0E, 0x64},       // P1                           �Զ�����
    {0x10, 0x14},       // S1                           �Զ�����
    {0x12, 0x8F},       // R2                           �Զ�����
    {0x14, 0x72},       // P2                           �Զ�����
    {0x16, 0x41},       // S2                           �Զ�����
    {0x18, 0x38},       // PaRamp           OOK����ʱ��                 Ĭ��ֵ
    {0x1A, 0x09},       // IrqParam1        IRQ����1
    {0x1C, 0x0D},       // IrqParam2        IRQ����2                  Ĭ��ֵ
    {0x1E, 0x00},       // RssiIrqThresh    RSSI��ֵ                  Ĭ��ֵ
    {0x20, 0xA3},       // RxParam1                                     Ĭ��ֵ
    {0x22, 0x38},       // RxParam2                                     Ĭ��ֵ
    {0x24, 0x2A},       // RxParam3         ���տ���3

    {0x26, 0x07},       // Res19            ����
    {0x68, 0x00},       // RssiValue        RSSIֵ,ֻ��

    {0x2A, 0x00},       // RxParam6                                     Ĭ��ֵ
    {0x2C, 0x25},       // SyncByte1        ͬ����1
    {0x2E, 0xC4},       // SyncByte2        ͬ����2
    {0x30, 0x00},       // SyncByte3        ͬ����3
    {0x32, 0x00},       // SyncByte4        ͬ����4

    {0x34, 0x7E},       // TxParam          �����˲�������
    {0x36, 0xFC},       // OscParam         �������
    {0x38, 0x3F},       // PktParam1        ������                     Ĭ��ֵ
    {0x3A, 0x00},       // NodeAddr         ������ַ                    Ĭ��ֵ
    {0x3C, 0xC8},       // PktParam3        ������1
    {0x3E, 0x00}        // PktParam4        ������2
};



uint8 SX1211_MODE;                          // ����ģʽ����Ϊ���ߡ����������������ա�����
uint8 SX1211_DATA_MODE;                     // ����ģʽ����ΪIO���ֽڡ����ݰ�
bool SX1211_in_TX;                          // SX1211 ������
bool SX1211_in_RX;                          // SX1211 ������
bool SX1211_in_FSK;                     // SX1211 FSKģʽ



uint8 initiate_SX1211 (void)                                            // ��ʼ��SX1211ģ��
{
    MCU_IO_INPUT (HAL_SX1211_CLKOUT_PORT, HAL_SX1211_CLKOUT_PIN, MCU_IO_PULLOFF);   // CLKOUT Ϊ���롣SX1211����ʱ��Լ10mS����ʱ���ɳ�ʼ�����ȴ�CLKOUT�ź�
    SX1211_SPI_Setup ();                                                    // ����SX1211 SPI��DMA0

    TA2CCR0 = ((uint32)HAL_CPU_CLOCK_KHz *5) /4;                            // ��ʱ10ms
    TA2CTL = TASSEL_2 + ID_3 + MC_1 + TAIE + TACLR;                         // SMCLK/8, up mode, interrupt disabled
    __low_power_mode_0();                                                   // LPM0����
    TA2CTL = 0;                                                             // �ر�TA2��ʱ
    TA2CCTL0 = 0;

    if (MCU_IO_GET(HAL_SX1211_CLKOUT_PORT, HAL_SX1211_CLKOUT_PIN) == 0x00)  // ��10�����δ��⵽CLKOUT������оƬ
    {
            uint8 _check[2];
            SX1211_PreSetCfg [Res19][0] |= SX1211_CFG_READ;                 // ������
            SX1211_Read_CFG_SPI ((uint32)&SX1211_PreSetCfg[Res19],(uint32)_check, 2);   // ��ȡ������
            while (SX1211_SPI_BUSY)     __low_power_mode_0();               // �ȴ�SPIͨ�����
            SX1211_PreSetCfg [Res19][0] &= ~SX1211_CFG_WRITE;               // д����

            if (_check[1] != 0x07)  return (FALSE);                         // оƬ�����ڻ���ϣ��˳�
    }   // ��֤ͨ����������ʼ��

    SX1211_Send_CFG_DMA ((uint32)&SX1211_PreSetCfg, sizeof(SX1211_PreSetCfg));  // ���ͳ�ʼ������

    halDigioIntSetEdge (&pinRadio_IRQ0, HAL_DIGIO_INT_RISING_EDGE);         // pinRadio_IRQ0�����ش���
    halDigioIntConnect (&pinRadio_IRQ0, IRQ0_isr);                          // ��ʼ���жϣ���������
    halDigioIntClear (&pinRadio_IRQ0);                                      // ���жϱ�־
    halDigioIntEnable (&pinRadio_IRQ0);                                     // �����ж�
    halDigioIntSetEdge (&pinRadio_IRQ1, HAL_DIGIO_INT_RISING_EDGE);         // pinRadio_IRQ1�����ش���
    halDigioIntConnect (&pinRadio_IRQ1, IRQ1_isr);                          // ��ʼ���жϣ���������
    halDigioIntClear (&pinRadio_IRQ1);                                      // ���жϱ�־
    halDigioIntEnable (&pinRadio_IRQ1);                                     // �����ж�


//  ��ʼ������
    SX1211_MODE = SX1211_MODE_STDBY;                                        // ��ʹ����ģʽ������
    SX1211_DATA_MODE = SX1211_DATA_MODE_PACKET;                             // ��ʹ����ģʽ�����ݰ�ģʽ
    SX1211_in_TX = FALSE;                                                   // δ����
    SX1211_in_RX = FALSE;                                                   // δ����
    SX1211_in_FSK = TRUE;                                                   // FSKģʽ

    return (TRUE);
}





//  McParam1
uint8 SX1211_Operating_Mode (uint8 _value)                                  // SX1211����ģʽѡ��
/*  ������
#define SX1211_MODE_SLEEP                       0x00
#define SX1211_MODE_STDBY                       0x20                        d
#define SX1211_MODE_FS                          0x40
#define SX1211_MODE_RX                          0x60
#define SX1211_MODE_TX                          0x80
*/
{
    if ((_value | SX1211_MODE_MASK)!= SX1211_MODE_MASK)     return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [McParam1][0] &= ~SX1211_CFG_WRITE;                    // д����
    SX1211_PreSetCfg [McParam1][1] &= ~SX1211_MODE_MASK;                    // ���ԭֵ
    SX1211_PreSetCfg [McParam1][1] |= _value;                               // ����ֵ

    SX1211_MODE = _value;                                                   // ����״̬

    if (_value == SX1211_MODE_TX)   SX1211_in_TX = TRUE;
    else    SX1211_in_TX = FALSE;
    if (_value == SX1211_MODE_RX)   SX1211_in_RX = TRUE;
    else    SX1211_in_RX = FALSE;                                           // ��־λ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                   // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[McParam1], 2);           // ����������

    return (TRUE);
}


uint8 SX1211_Freq_Select (uint8 _value)                                     // ��Ƶѡ��1��2
/*  ������
#define SX1211_FREQ_1                           0x00                        d
#define SX1211_FREQ_2                           0x01
*/
{
    if ((_value | SX1211_FREQ_MASK)!= SX1211_FREQ_MASK)     return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [McParam1][0] &= ~SX1211_CFG_WRITE;                    // д����
    SX1211_PreSetCfg [McParam1][1] &= ~SX1211_FREQ_MASK;                    // ���ԭֵ
    SX1211_PreSetCfg [McParam1][1] |= _value;                               // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[McParam1], 2);           // ����������

    return (TRUE);
}


//  McParam2
uint8 SX1211_DATA_Mode (uint8 _value)                                       // SX1211����ģʽ
/*  ������
#define SX1211_DATA_MODE_IO                     0x00                        d
#define SX1211_DATA_MODE_BYTE                   0x20
#define SX1211_DATA_MODE_PACKET                 0x04
*/
{
    if ((_value | SX1211_DATA_MODE_MASK)!= SX1211_DATA_MODE_MASK)       return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [McParam2][0] &= ~SX1211_CFG_WRITE;                    // д����
    SX1211_PreSetCfg [McParam2][1] &= ~SX1211_DATA_MODE_MASK;               // ���ԭֵ
    SX1211_PreSetCfg [McParam2][1] |= _value;                               // ����ֵ

    SX1211_DATA_MODE = _value;                                              // ��������ģʽ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[McParam2], 2);           // ����������

    return (TRUE);
}


uint8 SX1211_Modulation_Select (uint8 _value)                               // ����ģʽѡ��
/*  ������
#define SX1211_MOD_OOK                          0x40
#define SX1211_MOD_FSK                          0x80                        d
*/
{
    if ((_value | SX1211_MOD_MASK)!= SX1211_MOD_MASK)       return (FALSE); // ����������Χ������
    if (_value == SX1211_MOD_FSK)   SX1211_in_FSK = TRUE;                   // FSKģʽ
    else        SX1211_in_FSK = FALSE;                                      // OOKģʽ

    SX1211_PreSetCfg [McParam2][0] &= ~SX1211_CFG_WRITE;                    // д����
    SX1211_PreSetCfg [McParam2][1] &= ~SX1211_MOD_MASK;                     // ���ԭֵ
    SX1211_PreSetCfg [McParam2][1] |= _value;                               // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[McParam2], 2);           // ����������

    return (TRUE);
}


uint8 SX1211_IF_GAIN (uint8 _value)                                         // SX1211   IF����
/*  ������
#define SX1211_IF_GAIN_0                        0x00                        d
#define SX1211_IF_GAIN_M45                      0x01
#define SX1211_IF_GAIN_M90                      0x02
#define SX1211_IF_GAIN_M135                     0x03
*/
{
    if ((_value | SX1211_IF_GAIN_MASK)!= SX1211_IF_GAIN_MASK)       return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [McParam2][0] &= ~SX1211_CFG_WRITE;                    // д����
    SX1211_PreSetCfg [McParam2][1] &= ~SX1211_IF_GAIN_MASK;                 // ���ԭֵ
    SX1211_PreSetCfg [McParam2][1] |= _value;                               // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[McParam2], 2);           // ����������

    return (TRUE);
}


//  McParam6
uint8 SX1211_FIFO_Size (uint8 _value)                                       // FIFO�ߴ�
/*  ������
#define SX1211_FIFO_SIZE_16                     0x00                        d
#define SX1211_FIFO_SIZE_32                     0x40
#define SX1211_FIFO_SIZE_48                     0x80
#define SX1211_FIFO_SIZE_64                     0xC0
*/
{
    if ((_value | SX1211_FIFO_SIZE_MASK)!= SX1211_FIFO_SIZE_MASK)       return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [McParam6][0] &= ~SX1211_CFG_WRITE;                    // д����
    SX1211_PreSetCfg [McParam6][1] &= ~SX1211_FIFO_SIZE_MASK;               // ���ԭֵ
    SX1211_PreSetCfg [McParam6][1] |= _value;                               // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[McParam6], 2);           // ����������

    return (TRUE);
}


uint8 SX1211_FIFO_Threshold (uint8 _value)                                  // SX1211   FIFO�ж���ֵ    ������Χ��0-3F�� ��λ1
/*  ������
#define SX1211_FIFO_THRESH_15                   0x0F                        d
#define SX1211_FIFO_THRESH_31                   0x1F
#define SX1211_FIFO_THRESH_47                   0x2F
#define SX1211_FIFO_THRESH_63                   0x3F
*/
{
    if ((_value | SX1211_FIFO_THRESH_MASK)!= SX1211_FIFO_THRESH_MASK)       return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [McParam6][0] &= ~SX1211_CFG_WRITE;                    // д����
    SX1211_PreSetCfg [McParam6][1] &= ~SX1211_FIFO_THRESH_MASK;             // ���ԭֵ
    SX1211_PreSetCfg [McParam6][1] |= _value;                               // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[McParam6], 2);           // ����������

    return (TRUE);
}


//  PaRamp
uint8 SX1211_RA_Ramp (uint8 _value)                                         // SX1211   OOK��ƽ�仯ʱ��
/*  ������
#define SX1211_PA_RAMP_3                        0x00
#define SX1211_PA_RAMP_8                        0x08
#define SX1211_PA_RAMP_15                       0x10
#define SX1211_PA_RAMP_23                       0x18                        d
*/
{
    if ((_value | SX1211_PA_RAMP_MASK)!= SX1211_PA_RAMP_MASK)       return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [PaRamp][0] &= ~SX1211_CFG_WRITE;                      // д����
    SX1211_PreSetCfg [PaRamp][1] &= ~SX1211_PA_RAMP_MASK;                   // ���ԭֵ
    SX1211_PreSetCfg [PaRamp][1] |= _value;                                 // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[PaRamp], 2);             // ����������

    return (TRUE);
}


//  IrqParam1
uint8 SX1211_IRQ0_Select (uint8 _value)                                     // IRQ0ѡ��
/*  ������
//  ------------    IRQ0��IOģʽ   RX  ------------        d: SYNC         ע��IRQ0������RXģʽ
#define SX1211_IRQ0_IO_SYNC                     0x00                        d
#define SX1211_IRQ0_IO_RSSI                     0x40
//  ------------    IRQ0��BYTEģʽ RX  ------------        d: NULL         ע��IRQ0������RXģʽ
#define SX1211_IRQ0_BYTE_NULL                   0x00                        d
#define SX1211_IRQ0_BYTE_WRITE                  0x40
#define SX1211_IRQ0_BYTE_EMPTY                  0x80                    // Ҳ�����ڴ���ģʽ
#define SX1211_IRQ0_BYTE_SYNC                   0xC0
//  ------------    IRQ0��PACKETģʽ   RX  --------        d: PKTREADY     ע��IRQ0������RXģʽ
#define SX1211_IRQ0_PACKET_PKTREADY             0x00                        d
#define SX1211_IRQ0_PACKET_WRITE                0x40
#define SX1211_IRQ0_PACKET_EMPTY                0x80                    // Ҳ�����ڴ���ģʽ
#define SX1211_IRQ0_PACKET_SYNC                 0xC0                    //  Sync or Adrs_match
*/
{
    if ((_value | SX1211_IRQ0_RX_MASK)!= SX1211_IRQ0_RX_MASK)       return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [IrqParam1][0] &= ~SX1211_CFG_WRITE;                   // д����
    SX1211_PreSetCfg [IrqParam1][1] &= ~SX1211_IRQ0_RX_MASK;                // ���ԭֵ
    SX1211_PreSetCfg [IrqParam1][1] |= _value;                              // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[IrqParam1], 2);          // ����������

    return (TRUE);
}


uint8 SX1211_IRQ1_Select (uint8 _value)                                     // IRQ1ѡ��
/*  ������
//  ------------    IRQ1��IOģʽ   TX  RX  --------
//  �κ�ֵ��ΪDCLK
//  ------------    IRQ1��BYTEģʽ RX  ------------        d: NULL
#define SX1211_IRQ1_BYTE_NULL                   0x00                        d
#define SX1211_IRQ1_BYTE_FULL                   0x10                    // Ҳ�����ڴ���ģʽ
#define SX1211_IRQ1_BYTE_RSSI                   0x20
#define SX1211_IRQ1_BYTE_FIFOTHR                0x30                    // Ҳ�����ڴ���ģʽ
//  ------------    IRQ1��PACKETģʽ   RX  --------        d: CRCOK
#define SX1211_IRQ1_PACKET_CRCOK                0x00                        d
#define SX1211_IRQ1_PACKET_FULL                 0x10                    // Ҳ�����ڴ���ģʽ
#define SX1211_IRQ1_PACKET_RSSI                 0x20
#define SX1211_IRQ1_PACKET_FIFOTHR              0x30                    // Ҳ�����ڴ���ģʽ
--------------------------------------------------------------------------------
//  ------------    IRQ1��BYTE��PACKETģʽ  TX  ----        d: FULL
//  #define SX1211_IRQ1_BYTE_FULL                   0x00        �ض���     d
#define SX1211_IRQ1_BYTE_TXDONE                 0x08
*/
{
    if ((_value | SX1211_IRQ1_MASK)!= SX1211_IRQ1_MASK)     return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [IrqParam1][0] &= ~SX1211_CFG_WRITE;                   // д����
    SX1211_PreSetCfg [IrqParam1][1] &= ~SX1211_IRQ1_MASK;                   // ���ԭֵ
    SX1211_PreSetCfg [IrqParam1][1] |= _value;                              // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[IrqParam1], 2);          // ����������

    return (TRUE);
}


uint8 SX1211_FIFO_Status (void)                                             // FIFO ״̬��ȡ
/*  ������
#define SX1211_FIFO_FULL                        0x04
#define SX1211_FIFO_EMPTY                       0x02
#define SX1211_FIFO_OVERRUN                     0x01
*/
{
    uint8   _FIFO_Stat[2];

    SX1211_PreSetCfg [IrqParam1][0] |= SX1211_CFG_READ;                     // ������

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Read_CFG_SPI ((uint32)&SX1211_PreSetCfg[IrqParam1],(uint32)_FIFO_Stat, 2);   // ����������
    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������

    return (_FIFO_Stat[1] &0x07);                                           // ����״̬
}


//  IrqParam2
uint8 SX1211_FIFO_Fill (uint8 _value)                                       // SX1211   FIFOд��
/*  ������
#define SX1211_FIFOFILL_AUTO                    0x00                    // ��⵽ͬ���ּ���ʼ                d
#define SX1211_FIFOFILL_MANU                    0x80                    // FIFOFILL ����
--------------------------------------------------------------------------------
#define SX1211_FIFOFILL_STOP                    0x00                    // ֹͣ���趨AUTOλʱ��λ��Ч      d
#define SX1211_FIFOFILL_START                   0x40                    // ��ʼ���趨AUTOλʱ��λ��Ч
*/
{
    if ((_value | SX1211_FIFOFILL_MASK)!= SX1211_FIFOFILL_MASK)     return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [IrqParam2][0] &= ~SX1211_CFG_WRITE;                   // д����
    SX1211_PreSetCfg [IrqParam2][1] &= ~SX1211_FIFOFILL_MASK;               // ���ԭֵ
    SX1211_PreSetCfg [IrqParam2][1] |= _value;                              // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[IrqParam2], 2);          // ����������

    return (TRUE);
}


uint8 SX1211_TX_Start (uint8 _value)                                        // SX1211���Ϳ���
/*  ������
#define SX1211_TXSTART_BYTE_FULL                0x00                    // �ֽ�ģʽ��������ʱ���͡�IRQ0 = EMPTY                         d
#define SX1211_TXSTART_BYTE_NOTEMPTY            0x10                    // �ֽ�ģʽ������ǿշ��͡�IRQ0 = EMPTY
#define SX1211_TXSTART_PACKET_FULL              0x00                    // ���ݰ�ģʽ�������FIFO_THRESHʱ���͡�IRQ1 = FIFOTHR           d
#define SX1211_TXSTART_PACKET_NOTEMPTY          0x10                    // ���ݰ�ģʽ������ǿշ��͡�IRQ0 = EMPTY
*/
{
    if ((_value | SX1211_TXSTART_MASK)!= SX1211_TXSTART_MASK)       return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [IrqParam2][0] &= ~SX1211_CFG_WRITE;                   // д����
    SX1211_PreSetCfg [IrqParam2][1] &= ~SX1211_TXSTART_MASK;                // ���ԭֵ
    SX1211_PreSetCfg [IrqParam2][1] |= _value;                              // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[IrqParam2], 2);          // ����������

    return (TRUE);
}


void SX1211_RSSI_IRQ_Clear (void)                                           // ��RSSI_IRQ�ж�
{
    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[IrqParam2], 2);          // ����������
    return;
}


uint8 SX1211_PLL_OUT (uint8 _value)                                         // PLL_LOCK ����ѡ��
/*
#define SX1211_PLL_LOCK_OUT                     0x01                        d
#define SX1211_PLL_LOCK_DIS                     0x00
*/
{
    if ((_value | SX1211_PLL_LOCK_MASK)!= SX1211_PLL_LOCK_MASK)     return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [IrqParam2][0] &= ~SX1211_CFG_WRITE;                   // д����
    SX1211_PreSetCfg [IrqParam2][1] &= ~SX1211_PLL_LOCK_MASK;               // ���ԭֵ
    SX1211_PreSetCfg [IrqParam2][1] |= _value;                              // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[IrqParam2], 2);          // ����������

    return (TRUE);
}


//  RssiIrqThresh
void SX1211_RSSI_IRQ_Thresh (uint8 _value)                                  // дRSSI_IRQ��ֵ
{
    SX1211_PreSetCfg [RssiIrqThresh][0] &= ~SX1211_CFG_WRITE;               // д����
    SX1211_PreSetCfg [RssiIrqThresh][1] = _value;                           // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[RssiIrqThresh], 2);      // ����������
    return;
}


//  RxParam3
uint8 SX1211_POLYP (uint8 _value)                                           // ����������        ������OOK  RXģʽ
/*  ������
#define SX1211_POLYP_OFF                        0x00                        d
#define SX1211_POLYP_ON                         0x80
*/
{
//  if (SX1211_in_FSK)  return (FALSE);                                     // ��OOK��ֱ���˳�
    if ((_value | SX1211_POLYP_MASK)!= SX1211_POLYP_MASK)       return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [RxParam3][0] &= ~SX1211_CFG_WRITE;                    // д����
    SX1211_PreSetCfg [RxParam3][1] &= ~SX1211_POLYP_MASK;                   // ���ԭֵ
    SX1211_PreSetCfg [RxParam3][1] |= _value;                               // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[RxParam3], 2);           // ����������

    return (TRUE);
}


uint8 SX1211_BITSYNC (uint8 _value)                                         // λͬ��ѡ��        ������Continuous   RXģʽ
/*  ������
#define SX1211_BITSYNC_ON                       0x00                        d
#define SX1211_BITSYNC_OFF                      0x40
*/
{
//  if (SX1211_DATA_MODE != SX1211_DATA_MODE_IO)    return (FALSE);         // ��Continuous��ֱ���˳�
    if ((_value | SX1211_BITSYNC_MASK)!= SX1211_BITSYNC_MASK)       return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [RxParam3][0] &= ~SX1211_CFG_WRITE;                    // д����
    SX1211_PreSetCfg [RxParam3][1] &= ~SX1211_BITSYNC_MASK;                 // ���ԭֵ
    SX1211_PreSetCfg [RxParam3][1] |= _value;                               // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[RxParam3], 2);           // ����������

    return (TRUE);
}


uint8 SX1211_SYNC (uint8 _value)                                            // ��ͬ��ѡ��
/*  ������
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
//  if (SX1211_DATA_MODE != SX1211_DATA_MODE_PACKET)    return (FALSE);     // �����ݰ���ֱ���˳�
    if ((_value | SX1211_SYNC_MASK)!= SX1211_SYNC_MASK)     return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [RxParam3][0] &= ~SX1211_CFG_WRITE;                    // д����
    SX1211_PreSetCfg [RxParam3][1] &= ~SX1211_SYNC_MASK;                    // ���ԭֵ
    SX1211_PreSetCfg [RxParam3][1] |= _value;                               // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[RxParam3], 2);           // ����������

    return (TRUE);
}


//  RssiValue
uint8 SX1211_GET_RSSI (void)                                                // ��ȡRSSIֵ
{
    uint8 _rssi[2];

    SX1211_PreSetCfg [RssiValue][0] |= SX1211_CFG_READ;                     // ������

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Read_CFG_SPI ((uint32)&SX1211_PreSetCfg[RssiValue],(uint32)_rssi, 2);    // ����������
    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������

    return (_rssi[1]);                                                      // ����RSSIֵ
}


//  SyncByte
void SX1211_SYNC_BYTE (uint32 _value)                                       // �趨ͬ���֣���λΪ1����λΪ4
{
    SX1211_PreSetCfg [SyncByte1][0] &= ~SX1211_CFG_WRITE;                   // д����
    SX1211_PreSetCfg [SyncByte2][0] &= ~SX1211_CFG_WRITE;                   // д����
    SX1211_PreSetCfg [SyncByte3][0] &= ~SX1211_CFG_WRITE;                   // д����
    SX1211_PreSetCfg [SyncByte4][0] &= ~SX1211_CFG_WRITE;                   // д����


    SX1211_PreSetCfg [SyncByte1][1] = BREAK_UINT32 (_value, 3);
    SX1211_PreSetCfg [SyncByte2][1] = BREAK_UINT32 (_value, 2);
    SX1211_PreSetCfg [SyncByte3][1] = BREAK_UINT32 (_value, 1);
    SX1211_PreSetCfg [SyncByte4][1] = BREAK_UINT32 (_value, 0);

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[SyncByte1], 8);          // ����������
    return;
}


//  TxParam
uint8 SX1211_POUT_Select (uint8 _value)                                     // �������ѡ��
/*  ������
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
    if ((_value | SX1211_POUT_MASK)!= SX1211_POUT_MASK)     return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [TxParam][0] &= ~SX1211_CFG_WRITE;                     // д����
    SX1211_PreSetCfg [TxParam][1] &= ~SX1211_POUT_MASK;                     // ���ԭֵ
    SX1211_PreSetCfg [TxParam][1] |= _value;                                // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[TxParam], 2);            // ����������

    return (TRUE);
}


//  OscParam
uint8 SX1211_CLKOUT (uint8 _value)                                          // ���ʱ��
/*  ������
#define SX1211_CLKOUT_OFF                       0x00
#define SX1211_CLKOUT_ON                        0x80                        d
*/
{
    if ((_value | SX1211_CLKOUT_MASK)!= SX1211_CLKOUT_MASK)     return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [OscParam][0] &= ~SX1211_CFG_WRITE;                    // д����
    SX1211_PreSetCfg [OscParam][1] &= ~SX1211_CLKOUT_MASK;                  // ���ԭֵ
    SX1211_PreSetCfg [OscParam][1] |= _value;                               // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[OscParam], 2);           // ����������

    return (TRUE);
}


//  PktParam1
uint8 SX1211_Manchester (uint8 _value)                                      // ����˹�ر���
/*  ������
#define SX1211_MANCH_OFF                        0x00                        d
#define SX1211_MANCH_ON                         0x80
*/
{
    if ((_value | SX1211_MANCH_MASK)!= SX1211_MANCH_MASK)       return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [PktParam1][0] &= ~SX1211_CFG_WRITE;                   // д����
    SX1211_PreSetCfg [PktParam1][1] &= ~SX1211_MANCH_MASK;                  // ���ԭֵ
    SX1211_PreSetCfg [PktParam1][1] |= _value;                              // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[PktParam1], 2);          // ����������

    return (TRUE);
}


uint8 SX1211_Length (uint8 _value)                                          // ���ݰ�����    ���䳤ģʽ�£�RXʱΪ������ݳ��ȣ�TXʱ�޶��壩
{
    if (_value >=64)        return (FALSE);                                 // ����������Χ������

    SX1211_PreSetCfg [PktParam1][0] &= ~SX1211_CFG_WRITE;                   // д����
    SX1211_PreSetCfg [PktParam1][1] &= ~SX1211_LENGTH_MASK;                 // ���ԭֵ
    SX1211_PreSetCfg [PktParam1][1] |= _value;                              // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[PktParam1], 2);          // ����������

    return (TRUE);
}


//  NodeAddr
void SX1211_Address (uint8 _value)                                          // ���ݰ���ַ�ֽ�
{
    SX1211_PreSetCfg [NodeAddr][0] &= ~SX1211_CFG_WRITE;                    // д����
    SX1211_PreSetCfg [NodeAddr][1] = _value;                                // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[NodeAddr], 2);           // ����������

    return;
}


//  PktParam3
uint8 SX1211_Length_Set (uint8 _value)                                      // ���ݰ������趨
/*  ������
#define SX1211_LENGTH_FIX                       0x00                        d
#define SX1211_LENGTH_VAR                       0x80
*/
{
//  if (SX1211_DATA_MODE != SX1211_DATA_MODE_PACKET)    return (FALSE);     // �����ݰ���ֱ���˳�
    if ((_value | SX1211_LENGTH_SET_MASK)!= SX1211_LENGTH_SET_MASK)     return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [PktParam3][0] &= ~SX1211_CFG_WRITE;                   // д����
    SX1211_PreSetCfg [PktParam3][1] &= ~SX1211_LENGTH_SET_MASK;             // ���ԭֵ
    SX1211_PreSetCfg [PktParam3][1] |= _value;                              // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[PktParam3], 2);          // ����������

    return (TRUE);
}


uint8 SX1211_Preamble (uint8 _value)                                        // ǰ������
/*  ������
#define SX1211_PREA_1B                          0x00
#define SX1211_PREA_2B                          0x20
#define SX1211_PREA_3B                          0x40                        d
#define SX1211_PREA_4B                          0x60
*/
{
    if ((_value | SX1211_PREA_MASK)!= SX1211_PREA_MASK)     return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [PktParam3][0] &= ~SX1211_CFG_WRITE;                   // д����
    SX1211_PreSetCfg [PktParam3][1] &= ~SX1211_PREA_MASK;                   // ���ԭֵ
    SX1211_PreSetCfg [PktParam3][1] |= _value;                              // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[PktParam3], 2);          // ����������

    return (TRUE);
}


uint8 SX1211_Whitening (uint8 _value)                                       // �׻�
/*  ������
#define SX1211_WHITENING_OFF                    0x00                        d
#define SX1211_WHITENING_ON                     0x10
*/
{
//  if (SX1211_DATA_MODE != SX1211_DATA_MODE_PACKET)    return (FALSE);     // �����ݰ���ֱ���˳�
    if ((_value | SX1211_WHITENING_MASK)!= SX1211_WHITENING_MASK)       return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [PktParam3][0] &= ~SX1211_CFG_WRITE;                   // д����
    SX1211_PreSetCfg [PktParam3][1] &= ~SX1211_WHITENING_MASK;              // ���ԭֵ
    SX1211_PreSetCfg [PktParam3][1] |= _value;                              // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[PktParam3], 2);          // ����������

    return (TRUE);
}


uint8 SX1211_Address_FILT (uint8 _value)                                    // ���ݰ���ַ����
/*  ������
#define SX1211_ADDR_FILT_OFF                    0x00                        d
#define SX1211_ADDR_FILT_ON                     0x02
#define SX1211_ADDR_FILT_ON_0                   0x04                    // ��ַ��0x00
#define SX1211_ADDR_FILT_ON_0F                  0x06                    // ��ַ��0x00��0xFF
*/
{
    if ((_value | SX1211_ADDR_FILT_MASK)!= SX1211_ADDR_FILT_MASK)       return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [PktParam3][0] &= ~SX1211_CFG_WRITE;                   // д����
    SX1211_PreSetCfg [PktParam3][1] &= ~SX1211_ADDR_FILT_MASK;              // ���ԭֵ
    SX1211_PreSetCfg [PktParam3][1] |= _value;                              // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[PktParam3], 2);          // ����������

    return (TRUE);
}


uint8 SX1211_CRC (uint8 _value)                                             // ���ݰ�CRCУ��
/*  ������
#define SX1211_CRC_OFF                          0x00
#define SX1211_CRC_ON                           0x08                        d
*/
{
//  if (SX1211_DATA_MODE != SX1211_DATA_MODE_PACKET)    return (FALSE);     // �����ݰ���ֱ���˳�
    if ((_value | SX1211_CRC_MASK)!= SX1211_CRC_MASK)       return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [PktParam3][0] &= ~SX1211_CFG_WRITE;                   // д����
    SX1211_PreSetCfg [PktParam3][1] &= ~SX1211_CRC_MASK;                    // ���ԭֵ
    SX1211_PreSetCfg [PktParam3][1] |= _value;                              // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[PktParam3], 2);          // ����������

    return (TRUE);
}


//  PktParam4
uint8 SX1211_CRC_AUTOCLR (uint8 _value)                                     // CRCУ��ʧ��ֱ�����
/*  ������
#define SX1211_CRC_AUTOCLR_ON                   0x00                        d
#define SX1211_CRC_AUTOCLR_OFF                  0x80
*/
{
//  if (SX1211_DATA_MODE != SX1211_DATA_MODE_PACKET)    return (FALSE);     // �����ݰ���ֱ���˳�
    if ((_value | SX1211_CRC_AUTOCLR_MASK)!= SX1211_CRC_AUTOCLR_MASK)       return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [PktParam4][0] &= ~SX1211_CFG_WRITE;                   // д����
    SX1211_PreSetCfg [PktParam4][1] &= ~SX1211_CRC_AUTOCLR_MASK;            // ���ԭֵ
    SX1211_PreSetCfg [PktParam4][1] |= _value;                              // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[PktParam4], 2);          // ����������

    return (TRUE);
}


uint8 SX1211_FIFO_Standby (uint8 _value)                                    // ����ģʽFIFO
/*  ������
#define SX1211_STDBY_FIFO_WRITE                 0x00                        d
#define SX1211_STDBY_FIFO_READ                  0x40
*/
{
//  if (SX1211_DATA_MODE == SX1211_DATA_MODE_IO)    return (FALSE);         // Continuous��ֱ���˳�
    if ((_value | SX1211_STDBY_FIFO_MASK)!= SX1211_STDBY_FIFO_MASK)     return (FALSE); // ����������Χ������

    SX1211_PreSetCfg [PktParam4][0] &= ~SX1211_CFG_WRITE;                   // д����
    SX1211_PreSetCfg [PktParam4][1] &= ~SX1211_STDBY_FIFO_MASK;             // ���ԭֵ
    SX1211_PreSetCfg [PktParam4][1] |= _value;                              // ����ֵ

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_CFG_SPI ((uint32)&SX1211_PreSetCfg[PktParam4], 2);          // ����������

    return (TRUE);
}





uint8 SX1211_TX_Bytes (uint32 _address, uint8 _length)                      // �������ݣ���ַָ��ͳ���
{
    //Ӧ����ջ���
    if (SX1211_DATA_MODE == SX1211_DATA_MODE_IO)    return (FALSE);         // ����ģʽΪIO��ֱ���˳�
    if (_length >64)    _length = 64;                                       // �������֧��64�ֽ�

    // �ж�   ��ģʽ�л�

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Send_Bytes_DMA (_address, _length);                              // ��������

    return (TRUE);
}


void SX1211_RX_Bytes (uint8 *rxBuffer)                                      // ���ջ��������ݡ�������ʹ��DMA����˲����������ڲ�ͬ����ʵ�ϣ��ó����ڽ�FIFO��ֵ����rxBuffer��ͬʱ����rxBuffer��ԭֵд����FIFO
{
    if (SX1211_DATA_MODE == SX1211_DATA_MODE_IO)
    {   Rec_num = 0;    return;     }                                       // ����ģʽΪIO��ֱ���˳�

    uint8 PKT_Length;

    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
    SX1211_Read_Bytes_SPI ((uint32)rxBuffer,(uint32)&PKT_Length, 1);            // ��㷢һ��ʲô�����������ݳ���
    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������

    *rxBuffer = PKT_Length;
    rxBuffer++;

    SX1211_Read_Bytes_SPI ((uint32)rxBuffer,(uint32)rxBuffer, PKT_Length);  // ��������
    while (SX1211_SPI_BUSY)     __low_power_mode_0();                       // �ȴ�֮ǰ��SPI�������
}



HAL_ISR_FUNCTION(TA2_time_up,TIMER2_A1)
{
    TA2CTL &= ~TAIFG;
    __low_power_mode_off_on_exit();             // Clear CPUOFF bit from 0(SR)
}
