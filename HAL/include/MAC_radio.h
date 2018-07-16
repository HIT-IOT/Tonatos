#ifndef MAC_BASICRF_H
#define MAC_BASICRF_H

#include <types.h>

#define     send            0
#define     reply           1

#define     MAC_Payload_Length          64          // 单帧载荷长度


#define     MAC_RETX_CNT                3           // 重发次数



#define     LOCAL_ADDRESS               0x01        // 本机地址


/*******************************************************************************
    MAC状态           */
#define     MAC_IDLE                    0x00            // 无
#define     MAC_ERROR                   0x01            // 错误
#define     MAC_SLEEPING                0x02            // 休眠中
#define     MAC_SENDING                 0x04            // 发送中
#define     MAC_SEND_OVER               0x08            // 发送完毕
#define     MAC_RECEIVING               0x10            // 接收中
#define     MAC_RECV_ACK                0x20            // 接收到ACK
#define     MAC_RECV_CMD                0x40            // 接收到指令
#define     MAC_RECV_DATA               0x80            // 接收到数据


/*******************************************************************************
    发送帧 数据头                     调整数据结构可能导致内存地址不连续！！！
*/
typedef struct
{
uint8 to;                               // 数据送给..
uint8 from;                             // 数据源自..
uint8 via;                              // 数据经由..
uint8 stamp;                            // 邮戳
uint8 nwkMode;                          // 网络结构（默认广播：0）
uint8 powerPercent;                     // 本机电量
//  uint8 commPower;                        // 通信功率
//  uint8 RSSI;                             // 通信质量 （本机或对方）
} macDataHead;


/*******************************************************************************
    发送帧 数据（单帧）
*/
typedef struct
{
uint8 packetLength;                     // 发送：数据包总长度，不包含其自身
                                        // 接收：载荷字节数
macDataHead pktHeader;                  // 数据头（帧信息）
uint8 ctrlByte;                         // 帧功能标识
uint8 pktPayload[MAC_Payload_Length];   // 载荷               // 此处可考虑改为数组指针
} macDataPacket;


/*******************************************************************************
    变量声明
*/

extern uint8 mac_stat;                  // MAC状态
extern macDataPacket txbuf;             // 发送缓存
extern macDataPacket rxbuf;             // 接收缓存



/*******************************************************************************
    函数声明
*/

extern uint8 MAC_Init(void);                                        // 初始化MAC，启动后进入睡眠状态
extern void MAC_Off(void);                                      // 关闭MAC
extern uint8 MAC_Send_Data (uint8 _dstAddress, uint8 _dataLength, uint8 *_txBuffer);    // 发送数据，发送完毕后进入待机状态。数据应在64字节以内
extern uint8 MAC_Send_Out (void);                                   // MAC 发送程序。不要直接调用
extern uint8 MAC_Rx (void);                                     // 进入接收状态。接收到一个数据包后，自动进入接收状态
extern uint8 MAC_Idle (void);                                       // 进入待机状态
extern uint8 MAC_sleep (void);                                  // 进入睡眠状态



extern void IRQ0_isr (void);                                            // IRQ0中断处理程序
extern void IRQ1_isr (void);                                            // IRQ1中断处理程序

#endif
