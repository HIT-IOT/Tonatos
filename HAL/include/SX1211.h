
#ifndef _SX1211_
#define _SX1211_

#include <types.h>
#include <stdbool.h>
#include <SX1211_Board.h>

//  ------------    �Ĵ�����ַ   --------------------

#define McParam1        0x00
#define McParam2        0x01
#define Fdev            0x02
#define Bitrate         0x03
#define OokFloorThresh  0x04
#define McParam6        0x05
#define R1              0x06
#define P1              0x07
#define S1              0x08
#define R2              0x09
#define P2              0x0A
#define S2              0x0B
#define PaRamp          0x0C
#define IrqParam1       0x0D
#define IrqParam2       0x0E
#define RssiIrqThresh   0x0F
#define RxParam1        0x10
#define RxParam2        0x11
#define RxParam3        0x12
#define Res19           0x13
#define RssiValue       0x14
#define RxParam6        0x15
#define SyncByte1       0x16
#define SyncByte2       0x17
#define SyncByte3       0x18
#define SyncByte4       0x19
#define TxParam         0x1A
#define OscParam        0x1B
#define PktParam1       0x1C
#define NodeAddr        0x1D
#define PktParam3       0x1E
#define PktParam4       0x1F


/*
#define SX1211_bit_McParam1                     (0x00000001u)
#define SX1211_bit_McParam2                     (0x00000002u)
#define SX1211_bit_Fdev                         (0x00000004u)
#define SX1211_bit_Bitrate                      (0x00000008u)
#define SX1211_bit_OokFloorThresh               (0x00000010u)
#define SX1211_bit_McParam6                     (0x00000020u)
#define SX1211_bit_R1                           (0x00000040u)
#define SX1211_bit_P1                           (0x00000080u)
#define SX1211_bit_S1                           (0x00000100u)
#define SX1211_bit_R2                           (0x00000200u)
#define SX1211_bit_P2                           (0x00000400u)
#define SX1211_bit_S2                           (0x00000800u)
#define SX1211_bit_PaRamp                       (0x00001000u)
#define SX1211_bit_IrqParam1                    (0x00002000u)
#define SX1211_bit_IrqParam2                    (0x00004000u)
#define SX1211_bit_RssiIrqThresh                (0x00008000u)
#define SX1211_bit_RxParam1                     (0x00010000u)
#define SX1211_bit_RxParam2                     (0x00020000u)
#define SX1211_bit_RxParam3                     (0x00040000u)
#define SX1211_bit_Res19                        (0x00080000u)
#define SX1211_bit_RssiValue                    (0x00100000u)
#define SX1211_bit_RxParam6                     (0x00200000u)
#define SX1211_bit_SyncByte1                    (0x00400000u)
#define SX1211_bit_SyncByte2                    (0x00800000u)
#define SX1211_bit_SyncByte3                    (0x01000000u)
#define SX1211_bit_SyncByte4                    (0x02000000u)
#define SX1211_bit_TxParam                      (0x04000000u)
#define SX1211_bit_OscParam                     (0x08000000u)
#define SX1211_bit_PktParam1                    (0x10000000u)
#define SX1211_bit_NodeAddr                     (0x20000000u)
#define SX1211_bit_PktParam3                    (0x40000000u)
#define SX1211_bit_PktParam4                    (0x80000000u)
*/




//  ------------    ���ö�д    --------------------

#define SX1211_CFG_WRITE                        0x00
#define SX1211_CFG_READ                         0x40



//  McParam1
//  ------------    ����ģʽ    --------------------        d: STDBY

#define SX1211_MODE_SLEEP                       0x00
#define SX1211_MODE_STDBY                       0x20
#define SX1211_MODE_FS                          0x40
#define SX1211_MODE_RX                          0x60
#define SX1211_MODE_TX                          0x80
#define SX1211_MODE_MASK                        0xE0


//  ------------    ��Ƶѡ��    --------------------        d: 1

#define SX1211_FREQ_1                           0x00
#define SX1211_FREQ_2                           0x01
#define SX1211_FREQ_MASK                        0x01


//  McParam2
//  ------------    ����ѡ��    --------------------        d: FSK

#define SX1211_MOD_OOK                          0x40
#define SX1211_MOD_FSK                          0x80
#define SX1211_MOD_MASK                         0xC0


//  ------------    ����ģʽ    --------------------        d: IO

#define SX1211_DATA_MODE_IO                     0x00
#define SX1211_DATA_MODE_BYTE                   0x20
#define SX1211_DATA_MODE_PACKET                 0x04
#define SX1211_DATA_MODE_MASK                   0x24


//  ------------    IF����        --------------------        d: 0

#define SX1211_IF_GAIN_0                        0x00
#define SX1211_IF_GAIN_M45                      0x01
#define SX1211_IF_GAIN_M90                      0x02
#define SX1211_IF_GAIN_M135                     0x03
#define SX1211_IF_GAIN_MASK                     0x03


//  McParam6
//  ------------    FIFO�ߴ�  --------------------        d: 16

#define SX1211_FIFO_SIZE_16                     0x00
#define SX1211_FIFO_SIZE_32                     0x40
#define SX1211_FIFO_SIZE_48                     0x80
#define SX1211_FIFO_SIZE_64                     0xC0
#define SX1211_FIFO_SIZE_MASK                   0xC0


//  ------------    FIFO�ж���ֵ    ----------------        d: 15           ע����ֵ��ȡ 0-63������ֵ������ֻ�����ĸ�ѡ��

#define SX1211_FIFO_THRESH_15                   0x0F
#define SX1211_FIFO_THRESH_31                   0x1F
#define SX1211_FIFO_THRESH_47                   0x2F
#define SX1211_FIFO_THRESH_63                   0x3F
#define SX1211_FIFO_THRESH_MASK                 0x3F


//  PaRamp
//  ------------    OOK��ƽ�仯ʱ��       ------------        d: 23

#define SX1211_PA_RAMP_3                        0x00
#define SX1211_PA_RAMP_8                        0x08
#define SX1211_PA_RAMP_15                       0x10
#define SX1211_PA_RAMP_23                       0x18
#define SX1211_PA_RAMP_MASK                     0x18


//  IrqParam1
//  ------------    IRQ0��IOģʽ   RX  ------------        d: SYNC         ע��IRQ0������RXģʽ

#define SX1211_IRQ0_IO_SYNC                     0x00
#define SX1211_IRQ0_IO_RSSI                     0x40


//  ------------    IRQ0��BYTEģʽ RX  ------------        d: NULL         ע��IRQ0������RXģʽ

#define SX1211_IRQ0_BYTE_NULL                   0x00
#define SX1211_IRQ0_BYTE_WRITE                  0x40
#define SX1211_IRQ0_BYTE_EMPTY                  0x80                    // Ҳ�����ڴ���ģʽ
#define SX1211_IRQ0_BYTE_SYNC                   0xC0


//  ------------    IRQ0��PACKETģʽ   RX  --------        d: PKTREADY     ע��IRQ0������RXģʽ

#define SX1211_IRQ0_PACKET_PKTREADY             0x00
#define SX1211_IRQ0_PACKET_WRITE                0x40
#define SX1211_IRQ0_PACKET_EMPTY                0x80                    // Ҳ�����ڴ���ģʽ
#define SX1211_IRQ0_PACKET_SYNC                 0xC0                    //  Sync or Adrs_match

#define SX1211_IRQ0_RX_MASK                     0xC0


//  ------------    IRQ1��IOģʽ   TX  RX  --------
//  �κ�ֵ��ΪDCLK

//  ------------    IRQ1��BYTEģʽ RX  ------------        d: NULL

#define SX1211_IRQ1_BYTE_NULL                   0x00
#define SX1211_IRQ1_BYTE_FULL                   0x10                    // Ҳ�����ڴ���ģʽ
#define SX1211_IRQ1_BYTE_RSSI                   0x20
#define SX1211_IRQ1_BYTE_FIFOTHR                0x30                    // Ҳ�����ڴ���ģʽ


//  ------------    IRQ1��PACKETģʽ   RX  --------        d: CRCOK

#define SX1211_IRQ1_PACKET_CRCOK                0x00
#define SX1211_IRQ1_PACKET_FULL                 0x10                    // Ҳ�����ڴ���ģʽ
#define SX1211_IRQ1_PACKET_RSSI                 0x20
#define SX1211_IRQ1_PACKET_FIFOTHR              0x30                    // Ҳ�����ڴ���ģʽ


//  ------------    IRQ1��BYTE��PACKETģʽ  TX  ----        d: FULL

//  #define SX1211_IRQ1_BYTE_FULL                   0x00                �ض���
#define SX1211_IRQ1_BYTE_TXDONE                 0x08
#define SX1211_IRQ1_MASK                        0x38


//  ------------    FIFO״̬      ----------------        ֻ��

#define SX1211_FIFO_FULL                        0x04
#define SX1211_FIFO_EMPTY                       0x02
#define SX1211_FIFO_OVERRUN                     0x01


//  IrqParam2
//  ------------    FIFOд��      ----------------        d: AUTO         ע��������BYTEģʽ

#define SX1211_FIFOFILL_AUTO                    0x00                    // ��⵽ͬ���ּ���ʼ
#define SX1211_FIFOFILL_MANU                    0x80                    // FIFOFILL ����

#define SX1211_FIFOFILL_STOP                    0x00                    // ֹͣ���趨AUTOλʱ��λ��Ч
#define SX1211_FIFOFILL_START                   0x40                    // ��ʼ���趨AUTOλʱ��λ��Ч
#define SX1211_FIFOFILL_MASK                    0xC0


//  ------------    ���Ϳ���    --------------------        d: FULL

#define SX1211_TXSTART_BYTE_FULL                0x00                    // �ֽ�ģʽ��������ʱ���͡�IRQ0 = EMPTY
#define SX1211_TXSTART_BYTE_NOTEMPTY            0x10                    // �ֽ�ģʽ������ǿշ��͡�IRQ0 = EMPTY
#define SX1211_TXSTART_PACKET_FULL              0x00                    // ���ݰ�ģʽ�������FIFO_THRESHʱ���͡�IRQ1 = FIFOTHR
#define SX1211_TXSTART_PACKET_NOTEMPTY          0x10                    // ���ݰ�ģʽ������ǿշ��͡�IRQ0 = EMPTY
#define SX1211_TXSTART_MASK                     0x10


//  ------------    RSSI�ж�  --------------------
#define SX1211_RSSI_IRQ_CLEAR                   0x04

//  ------------    PLL_LOCK    --------------------
#define SX1211_PLL_LOCK_OUT                     0x01
#define SX1211_PLL_LOCK_DIS                     0x00
#define SX1211_PLL_LOCK_MASK                    0x03





//  RxParam3
//  ------------    PolypFilt   --------------------        d: OFF          ��OOK RXģʽ

#define SX1211_POLYP_OFF                        0x00
#define SX1211_POLYP_ON                         0x80
#define SX1211_POLYP_MASK                       0x80


//  ------------    λͬ��     --------------------        d: ON           ��IO RXģʽ

#define SX1211_BITSYNC_ON                       0x00
#define SX1211_BITSYNC_OFF                      0x40
#define SX1211_BITSYNC_MASK                     0x40


//  ------------    ��ͬ��     --------------------        d: OFF

#define SX1211_SYNC_OFF                         0x00
#define SX1211_SYNC_1B                          0x20
#define SX1211_SYNC_2B                          0x28
#define SX1211_SYNC_3B                          0x30
#define SX1211_SYNC_4B                          0x38

//  ------------    ��ͬ���ݴ�   --------------------        d: 0

#define SX1211_SYNC_TOL_0                       0x00
#define SX1211_SYNC_TOL_1                       0x02
#define SX1211_SYNC_TOL_2                       0x04
#define SX1211_SYNC_TOL_3                       0x06

#define SX1211_SYNC_MASK                        0x3E





//  TxParam
//  ------------    �������    --------------------        d: 10

#define SX1211_POUT_13                          0x00
#define SX1211_POUT_10                          0x02
#define SX1211_POUT_7                           0x04
#define SX1211_POUT_4                           0x06
#define SX1211_POUT_1                           0x08
#define SX1211_POUT_M2                          0x0A
#define SX1211_POUT_M5                          0x0C
#define SX1211_POUT_M8                          0x0E
#define SX1211_POUT_MASK                        0x0E


//  OscParam
//  ------------    CLKOUT      --------------------        d: OFF

#define SX1211_CLKOUT_OFF                       0x00
#define SX1211_CLKOUT_ON                        0x80
#define SX1211_CLKOUT_MASK                      0x80





//  PktParam1
//  ------------    ����˹�ر���  ----------------        d: OFF

#define SX1211_MANCH_OFF                        0x00
#define SX1211_MANCH_ON                         0x80
#define SX1211_MANCH_MASK                       0x80

#define SX1211_LENGTH_MASK                      0x7F


//  PktParam3
//  ------------    ����/�䳤       ----------------        d: FIX

#define SX1211_LENGTH_FIX                       0x00
#define SX1211_LENGTH_VAR                       0x80
#define SX1211_LENGTH_SET_MASK                  0x80


//  ------------    ǰ������    --------------------        d: 3B

#define SX1211_PREA_1B                          0x00
#define SX1211_PREA_2B                          0x20
#define SX1211_PREA_3B                          0x40
#define SX1211_PREA_4B                          0x60
#define SX1211_PREA_MASK                        0x60


//  ------------    �׻�  ------------------------        d: OFF

#define SX1211_WHITENING_OFF                    0x00
#define SX1211_WHITENING_ON                     0x10
#define SX1211_WHITENING_MASK                   0x10


//  ------------    CRC     ------------------------        d: OFF

#define SX1211_CRC_OFF                          0x00
#define SX1211_CRC_ON                           0x08
#define SX1211_CRC_MASK                         0x08


//  ------------    ��ַƥ��    --------------------        d: OFF

#define SX1211_ADDR_FILT_OFF                    0x00
#define SX1211_ADDR_FILT_ON                     0x02
#define SX1211_ADDR_FILT_ON_0                   0x04                    // ��ַ��0x00
#define SX1211_ADDR_FILT_ON_0F                  0x06                    // ��ַ��0x00��0xFF
#define SX1211_ADDR_FILT_MASK                   0x06


//  PktParam4
//  ------------    CRC�Զ����     ----------------        d: ON

#define SX1211_CRC_AUTOCLR_ON                   0x00
#define SX1211_CRC_AUTOCLR_OFF                  0x80
#define SX1211_CRC_AUTOCLR_MASK                 0x80



//  ------------    ����ģʽFIFO    ----------------        d: WRITE        ����ģʽFIFO����д���Ƕ�

#define SX1211_STDBY_FIFO_WRITE                 0x00
#define SX1211_STDBY_FIFO_READ                  0x40
#define SX1211_STDBY_FIFO_MASK                  0x40





extern uint8 SX1211_MODE;                           // ����ģʽ����Ϊ���ߡ����������������ա�����
extern uint8 SX1211_DATA_MODE;                      // ����ģʽ����ΪIO���ֽڡ����ݰ�
extern bool SX1211_in_TX;                           // SX1211 ������
extern bool SX1211_in_RX;                           // SX1211 ������
extern bool SX1211_in_FSK;                          // SX1211 FSKģʽ





extern uint8 initiate_SX1211 (void);                                    // ��ʼ��SX1211ģ��          ע���ó����֧���ϵ���һ�γ�ʼ�����ظ����û᷵��FALSE


extern uint8 SX1211_Operating_Mode (uint8 _value);                      // SX1211����ģʽѡ��
extern uint8 SX1211_Freq_Select (uint8 _value);                         // ��Ƶѡ��1��2
extern uint8 SX1211_DATA_Mode (uint8 _value);                           // SX1211����ģʽ
extern uint8 SX1211_Modulation_Select (uint8 _value);                   // ����ģʽѡ��
extern uint8 SX1211_IF_GAIN (uint8 _value);                             // SX1211   IF����
extern uint8 SX1211_FIFO_Size (uint8 _value);                           // FIFO�ߴ�
extern uint8 SX1211_FIFO_Threshold (uint8 _value);                      // SX1211   FIFO�ж���ֵ    ������Χ��0-3F�� ��λ1
extern uint8 SX1211_RA_Ramp (uint8 _value);                             // SX1211   OOK��ƽ�仯ʱ��
extern uint8 SX1211_IRQ0_Select (uint8 _value);                         // IRQ0ѡ��
extern uint8 SX1211_IRQ1_Select (uint8 _value);                         // IRQ1ѡ��
extern uint8 SX1211_FIFO_Fill (uint8 _value);                           // SX1211   FIFOд��
extern uint8 SX1211_TX_Start (uint8 _value);                            // SX1211���Ϳ���
extern void SX1211_RSSI_IRQ_Clear (void);                               // ��RSSI_IRQ�ж�
extern uint8 SX1211_PLL_OUT (uint8 _value);                             // PLL_LOCK ����ѡ��
extern void SX1211_RSSI_IRQ_Thresh (uint8 _value);                      // дRSSI_IRQ��ֵ
extern uint8 SX1211_POLYP (uint8 _value);                               // ����������        ������OOK  RXģʽ
extern uint8 SX1211_BITSYNC (uint8 _value);                             // λͬ��ѡ��        ������Continuous   RXģʽ
extern uint8 SX1211_SYNC (uint8 _value);                                // ��ͬ��ѡ��
extern uint8 SX1211_GET_RSSI (void);                                    // ��ȡRSSIֵ
extern void SX1211_SYNC_BYTE (uint32 _value);                           // �趨ͬ���֣���λΪ1����λΪ4
extern uint8 SX1211_POUT_Select (uint8 _value);                         // �������ѡ��
extern uint8 SX1211_CLKOUT (uint8 _value);                              // ���ʱ��
extern uint8 SX1211_Manchester (uint8 _value);                          // ����˹�ر���
extern uint8 SX1211_Length (uint8 _value);                              // ���ݰ�����    ���䳤ģʽ�£�RXʱΪ������ݳ��ȣ�TXʱ�޶��壩
extern void SX1211_Address (uint8 _value);                              // ���ݰ���ַ�ֽ�
extern uint8 SX1211_Length_Set (uint8 _value);                          // ���ݰ������趨
extern uint8 SX1211_Preamble (uint8 _value);                            // ǰ������
extern uint8 SX1211_Whitening (uint8 _value);                           // �׻�
extern uint8 SX1211_Address_FILT (uint8 _value);                        // ���ݰ���ַ����
extern uint8 SX1211_CRC (uint8 _value);                                 // ���ݰ�CRCУ��
extern uint8 SX1211_CRC_AUTOCLR (uint8 _value);                         // CRCУ��ʧ��ֱ�����
extern uint8 SX1211_FIFO_Standby (uint8 _value) ;                       // ����ģʽFIFO



extern uint8 SX1211_TX_Bytes (uint32 _address, uint8 _length);          // �������ݣ���ַָ��ͳ���
extern void SX1211_RX_Bytes (uint8 *rxBuffer);                          // ���ջ��������ݡ�������ʹ��DMA����˲����������ڲ�ͬ




#endif
