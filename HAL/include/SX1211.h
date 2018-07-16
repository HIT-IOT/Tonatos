
#ifndef _SX1211_
#define _SX1211_

#include <types.h>
#include <stdbool.h>
#include <SX1211_Board.h>

//  ------------    寄存器地址   --------------------

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




//  ------------    配置读写    --------------------

#define SX1211_CFG_WRITE                        0x00
#define SX1211_CFG_READ                         0x40



//  McParam1
//  ------------    工作模式    --------------------        d: STDBY

#define SX1211_MODE_SLEEP                       0x00
#define SX1211_MODE_STDBY                       0x20
#define SX1211_MODE_FS                          0x40
#define SX1211_MODE_RX                          0x60
#define SX1211_MODE_TX                          0x80
#define SX1211_MODE_MASK                        0xE0


//  ------------    跳频选择    --------------------        d: 1

#define SX1211_FREQ_1                           0x00
#define SX1211_FREQ_2                           0x01
#define SX1211_FREQ_MASK                        0x01


//  McParam2
//  ------------    调制选择    --------------------        d: FSK

#define SX1211_MOD_OOK                          0x40
#define SX1211_MOD_FSK                          0x80
#define SX1211_MOD_MASK                         0xC0


//  ------------    数据模式    --------------------        d: IO

#define SX1211_DATA_MODE_IO                     0x00
#define SX1211_DATA_MODE_BYTE                   0x20
#define SX1211_DATA_MODE_PACKET                 0x04
#define SX1211_DATA_MODE_MASK                   0x24


//  ------------    IF增益        --------------------        d: 0

#define SX1211_IF_GAIN_0                        0x00
#define SX1211_IF_GAIN_M45                      0x01
#define SX1211_IF_GAIN_M90                      0x02
#define SX1211_IF_GAIN_M135                     0x03
#define SX1211_IF_GAIN_MASK                     0x03


//  McParam6
//  ------------    FIFO尺寸  --------------------        d: 16

#define SX1211_FIFO_SIZE_16                     0x00
#define SX1211_FIFO_SIZE_32                     0x40
#define SX1211_FIFO_SIZE_48                     0x80
#define SX1211_FIFO_SIZE_64                     0xC0
#define SX1211_FIFO_SIZE_MASK                   0xC0


//  ------------    FIFO中断阈值    ----------------        d: 15           注：该值可取 0-63中任意值，这里只给出四个选项

#define SX1211_FIFO_THRESH_15                   0x0F
#define SX1211_FIFO_THRESH_31                   0x1F
#define SX1211_FIFO_THRESH_47                   0x2F
#define SX1211_FIFO_THRESH_63                   0x3F
#define SX1211_FIFO_THRESH_MASK                 0x3F


//  PaRamp
//  ------------    OOK电平变化时间       ------------        d: 23

#define SX1211_PA_RAMP_3                        0x00
#define SX1211_PA_RAMP_8                        0x08
#define SX1211_PA_RAMP_15                       0x10
#define SX1211_PA_RAMP_23                       0x18
#define SX1211_PA_RAMP_MASK                     0x18


//  IrqParam1
//  ------------    IRQ0：IO模式   RX  ------------        d: SYNC         注：IRQ0仅用于RX模式

#define SX1211_IRQ0_IO_SYNC                     0x00
#define SX1211_IRQ0_IO_RSSI                     0x40


//  ------------    IRQ0：BYTE模式 RX  ------------        d: NULL         注：IRQ0仅用于RX模式

#define SX1211_IRQ0_BYTE_NULL                   0x00
#define SX1211_IRQ0_BYTE_WRITE                  0x40
#define SX1211_IRQ0_BYTE_EMPTY                  0x80                    // 也适用于待机模式
#define SX1211_IRQ0_BYTE_SYNC                   0xC0


//  ------------    IRQ0：PACKET模式   RX  --------        d: PKTREADY     注：IRQ0仅用于RX模式

#define SX1211_IRQ0_PACKET_PKTREADY             0x00
#define SX1211_IRQ0_PACKET_WRITE                0x40
#define SX1211_IRQ0_PACKET_EMPTY                0x80                    // 也适用于待机模式
#define SX1211_IRQ0_PACKET_SYNC                 0xC0                    //  Sync or Adrs_match

#define SX1211_IRQ0_RX_MASK                     0xC0


//  ------------    IRQ1：IO模式   TX  RX  --------
//  任何值均为DCLK

//  ------------    IRQ1：BYTE模式 RX  ------------        d: NULL

#define SX1211_IRQ1_BYTE_NULL                   0x00
#define SX1211_IRQ1_BYTE_FULL                   0x10                    // 也适用于待机模式
#define SX1211_IRQ1_BYTE_RSSI                   0x20
#define SX1211_IRQ1_BYTE_FIFOTHR                0x30                    // 也适用于待机模式


//  ------------    IRQ1：PACKET模式   RX  --------        d: CRCOK

#define SX1211_IRQ1_PACKET_CRCOK                0x00
#define SX1211_IRQ1_PACKET_FULL                 0x10                    // 也适用于待机模式
#define SX1211_IRQ1_PACKET_RSSI                 0x20
#define SX1211_IRQ1_PACKET_FIFOTHR              0x30                    // 也适用于待机模式


//  ------------    IRQ1：BYTE、PACKET模式  TX  ----        d: FULL

//  #define SX1211_IRQ1_BYTE_FULL                   0x00                重定义
#define SX1211_IRQ1_BYTE_TXDONE                 0x08
#define SX1211_IRQ1_MASK                        0x38


//  ------------    FIFO状态      ----------------        只读

#define SX1211_FIFO_FULL                        0x04
#define SX1211_FIFO_EMPTY                       0x02
#define SX1211_FIFO_OVERRUN                     0x01


//  IrqParam2
//  ------------    FIFO写入      ----------------        d: AUTO         注：仅用于BYTE模式

#define SX1211_FIFOFILL_AUTO                    0x00                    // 检测到同步字即开始
#define SX1211_FIFOFILL_MANU                    0x80                    // FIFOFILL 控制

#define SX1211_FIFOFILL_STOP                    0x00                    // 停止。设定AUTO位时此位无效
#define SX1211_FIFOFILL_START                   0x40                    // 开始。设定AUTO位时此位无效
#define SX1211_FIFOFILL_MASK                    0xC0


//  ------------    发送控制    --------------------        d: FULL

#define SX1211_TXSTART_BYTE_FULL                0x00                    // 字节模式，缓存满时发送。IRQ0 = EMPTY
#define SX1211_TXSTART_BYTE_NOTEMPTY            0x10                    // 字节模式，缓存非空发送。IRQ0 = EMPTY
#define SX1211_TXSTART_PACKET_FULL              0x00                    // 数据包模式，缓存≥FIFO_THRESH时发送。IRQ1 = FIFOTHR
#define SX1211_TXSTART_PACKET_NOTEMPTY          0x10                    // 数据包模式，缓存非空发送。IRQ0 = EMPTY
#define SX1211_TXSTART_MASK                     0x10


//  ------------    RSSI中断  --------------------
#define SX1211_RSSI_IRQ_CLEAR                   0x04

//  ------------    PLL_LOCK    --------------------
#define SX1211_PLL_LOCK_OUT                     0x01
#define SX1211_PLL_LOCK_DIS                     0x00
#define SX1211_PLL_LOCK_MASK                    0x03





//  RxParam3
//  ------------    PolypFilt   --------------------        d: OFF          仅OOK RX模式

#define SX1211_POLYP_OFF                        0x00
#define SX1211_POLYP_ON                         0x80
#define SX1211_POLYP_MASK                       0x80


//  ------------    位同步     --------------------        d: ON           仅IO RX模式

#define SX1211_BITSYNC_ON                       0x00
#define SX1211_BITSYNC_OFF                      0x40
#define SX1211_BITSYNC_MASK                     0x40


//  ------------    字同步     --------------------        d: OFF

#define SX1211_SYNC_OFF                         0x00
#define SX1211_SYNC_1B                          0x20
#define SX1211_SYNC_2B                          0x28
#define SX1211_SYNC_3B                          0x30
#define SX1211_SYNC_4B                          0x38

//  ------------    字同步容错   --------------------        d: 0

#define SX1211_SYNC_TOL_0                       0x00
#define SX1211_SYNC_TOL_1                       0x02
#define SX1211_SYNC_TOL_2                       0x04
#define SX1211_SYNC_TOL_3                       0x06

#define SX1211_SYNC_MASK                        0x3E





//  TxParam
//  ------------    输出功率    --------------------        d: 10

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
//  ------------    曼彻斯特编码  ----------------        d: OFF

#define SX1211_MANCH_OFF                        0x00
#define SX1211_MANCH_ON                         0x80
#define SX1211_MANCH_MASK                       0x80

#define SX1211_LENGTH_MASK                      0x7F


//  PktParam3
//  ------------    定长/变长       ----------------        d: FIX

#define SX1211_LENGTH_FIX                       0x00
#define SX1211_LENGTH_VAR                       0x80
#define SX1211_LENGTH_SET_MASK                  0x80


//  ------------    前导长度    --------------------        d: 3B

#define SX1211_PREA_1B                          0x00
#define SX1211_PREA_2B                          0x20
#define SX1211_PREA_3B                          0x40
#define SX1211_PREA_4B                          0x60
#define SX1211_PREA_MASK                        0x60


//  ------------    白化  ------------------------        d: OFF

#define SX1211_WHITENING_OFF                    0x00
#define SX1211_WHITENING_ON                     0x10
#define SX1211_WHITENING_MASK                   0x10


//  ------------    CRC     ------------------------        d: OFF

#define SX1211_CRC_OFF                          0x00
#define SX1211_CRC_ON                           0x08
#define SX1211_CRC_MASK                         0x08


//  ------------    地址匹配    --------------------        d: OFF

#define SX1211_ADDR_FILT_OFF                    0x00
#define SX1211_ADDR_FILT_ON                     0x02
#define SX1211_ADDR_FILT_ON_0                   0x04                    // 地址或0x00
#define SX1211_ADDR_FILT_ON_0F                  0x06                    // 地址或0x00或0xFF
#define SX1211_ADDR_FILT_MASK                   0x06


//  PktParam4
//  ------------    CRC自动清除     ----------------        d: ON

#define SX1211_CRC_AUTOCLR_ON                   0x00
#define SX1211_CRC_AUTOCLR_OFF                  0x80
#define SX1211_CRC_AUTOCLR_MASK                 0x80



//  ------------    待机模式FIFO    ----------------        d: WRITE        待机模式FIFO用于写还是读

#define SX1211_STDBY_FIFO_WRITE                 0x00
#define SX1211_STDBY_FIFO_READ                  0x40
#define SX1211_STDBY_FIFO_MASK                  0x40





extern uint8 SX1211_MODE;                           // 工作模式，分为休眠、待机、就绪、接收、发送
extern uint8 SX1211_DATA_MODE;                      // 数据模式，分为IO、字节、数据包
extern bool SX1211_in_TX;                           // SX1211 发送中
extern bool SX1211_in_RX;                           // SX1211 接收中
extern bool SX1211_in_FSK;                          // SX1211 FSK模式





extern uint8 initiate_SX1211 (void);                                    // 初始化SX1211模块          注：该程序仅支持上电后第一次初始化，重复调用会返回FALSE


extern uint8 SX1211_Operating_Mode (uint8 _value);                      // SX1211工作模式选择
extern uint8 SX1211_Freq_Select (uint8 _value);                         // 跳频选择，1或2
extern uint8 SX1211_DATA_Mode (uint8 _value);                           // SX1211数据模式
extern uint8 SX1211_Modulation_Select (uint8 _value);                   // 调制模式选择
extern uint8 SX1211_IF_GAIN (uint8 _value);                             // SX1211   IF增益
extern uint8 SX1211_FIFO_Size (uint8 _value);                           // FIFO尺寸
extern uint8 SX1211_FIFO_Threshold (uint8 _value);                      // SX1211   FIFO中断阈值    参数范围：0-3F， 单位1
extern uint8 SX1211_RA_Ramp (uint8 _value);                             // SX1211   OOK电平变化时间
extern uint8 SX1211_IRQ0_Select (uint8 _value);                         // IRQ0选择
extern uint8 SX1211_IRQ1_Select (uint8 _value);                         // IRQ1选择
extern uint8 SX1211_FIFO_Fill (uint8 _value);                           // SX1211   FIFO写入
extern uint8 SX1211_TX_Start (uint8 _value);                            // SX1211发送控制
extern void SX1211_RSSI_IRQ_Clear (void);                               // 清RSSI_IRQ中断
extern uint8 SX1211_PLL_OUT (uint8 _value);                             // PLL_LOCK 功能选择
extern void SX1211_RSSI_IRQ_Thresh (uint8 _value);                      // 写RSSI_IRQ阈值
extern uint8 SX1211_POLYP (uint8 _value);                               // ？？？？？        仅用于OOK  RX模式
extern uint8 SX1211_BITSYNC (uint8 _value);                             // 位同步选择        仅用于Continuous   RX模式
extern uint8 SX1211_SYNC (uint8 _value);                                // 字同步选择
extern uint8 SX1211_GET_RSSI (void);                                    // 获取RSSI值
extern void SX1211_SYNC_BYTE (uint32 _value);                           // 设定同步字，高位为1，低位为4
extern uint8 SX1211_POUT_Select (uint8 _value);                         // 输出功率选择
extern uint8 SX1211_CLKOUT (uint8 _value);                              // 输出时钟
extern uint8 SX1211_Manchester (uint8 _value);                          // 曼彻斯特编码
extern uint8 SX1211_Length (uint8 _value);                              // 数据包长度    （变长模式下，RX时为最大数据长度，TX时无定义）
extern void SX1211_Address (uint8 _value);                              // 数据包地址字节
extern uint8 SX1211_Length_Set (uint8 _value);                          // 数据包长度设定
extern uint8 SX1211_Preamble (uint8 _value);                            // 前导长度
extern uint8 SX1211_Whitening (uint8 _value);                           // 白化
extern uint8 SX1211_Address_FILT (uint8 _value);                        // 数据包地址过滤
extern uint8 SX1211_CRC (uint8 _value);                                 // 数据包CRC校验
extern uint8 SX1211_CRC_AUTOCLR (uint8 _value);                         // CRC校验失败直接清除
extern uint8 SX1211_FIFO_Standby (uint8 _value) ;                       // 待机模式FIFO



extern uint8 SX1211_TX_Bytes (uint32 _address, uint8 _length);          // 发送数据，地址指针和长度
extern void SX1211_RX_Bytes (uint8 *rxBuffer);                          // 接收缓存中数据。因无需使用DMA，因此参数类型与众不同




#endif
