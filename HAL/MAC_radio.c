/*
    MAC 协议版本 v0.1

更改日期：2017.1.9
更改功能：简化为无源MAC初始版


    主从广播通信，无中继
    仅考虑单数据帧(变长)，未涉及多帧操作，可扩展
    未考虑IO模式和字节模式

    地址为8位，可扩展为16位

    当连续收到多个数据包时，如果处理速度不够，会有丢包的可能
    本程序不完善



    发送时，如产生TX_DONE中断信号，此时最后一位正在发送中，应有一点延时待其发送完毕。


    mac_stat：MAC状态
        0x00：无
        0x01：错误
        0x02：休眠中
        0x04：
        0x08：发送中
        0x10：接收中
        0x20：接收到ACK
        0x40：接收到指令
        0x80：接收到数据




系统变量：
        mac_stat：MAC工作状态


底层使用SX1211。IEA也可使用CC系列


*/

#include <MAC_radio.h>
#include <SX1211.h>
#include <macros.h>
#include <defs.h>
#include <types.h>
#include <hal.h>
#include <msp430.h>

macDataPacket txbuf;                                // 发送缓存
macDataPacket rxbuf;                                // 接收缓存


uint8 mac_stat = MAC_ERROR;                         // MAC状态
uint8 rx_buff_size;                                 // 接收缓存大小 / 接收到的字节个数



uint8 MAC_Init(void)                                // 初始化MAC，启动后进入睡眠状态
{
    if (initiate_SX1211() == FALSE)                 // 初始化SX1211
    {                                               // 如不成功，
        mac_stat = MAC_ERROR;                       // MAC状态：故障
        return (FALSE);                             // 返回错误
    }

//  SX1211_Address_FILT (SX1211_ADDR_FILT_ON_0);    // 仅接收本机地址或0x00
//  SX1211_Operating_Mode (SX1211_MODE_SLEEP);      // 睡眠
    mac_stat = MAC_SLEEPING;                        // MAC状态：睡眠

    txbuf.pktHeader.via = LOCAL_ADDRESS;
    txbuf.pktHeader.stamp = 0x00;
    txbuf.pktHeader.nwkMode = 0x00;
    txbuf.ctrlByte = 0x00;

    return (TRUE);                                  // 初始化成功
}

void MAC_Off(void)                                  // 关闭MAC
{
    mac_stat = MAC_ERROR;                           // MAC状态：故障
    SX1211_Operating_Mode(SX1211_MODE_SLEEP);      // 睡眠
}



uint8 MAC_Send_Data (uint8 _dstAddress, uint8 _dataLength, uint8 *_txBuffer)    // 发送数据，发送完毕后进入待机状态。数据应在64字节以内
{
    uint8 i;

    if (mac_stat == MAC_ERROR)  return (FALSE);             // 如模块错误，退出
    if (_dataLength > MAC_Payload_Length)   return (FALSE); // 待完善，暂时未考虑多帧

    txbuf.packetLength = sizeof(macDataHead) + 1 + _dataLength; // 数据头长度 + 帧功能标识 + 载荷长度
    txbuf.pktHeader.to = _dstAddress;
    txbuf.pktHeader.nwkMode = 0;                                // 暂时仅考虑单帧
    txbuf.pktHeader.powerPercent = sys_status.Pri_voltage /30;  // 以3V为100%
    txbuf.ctrlByte = 0x00;

    for (i=0; i<_dataLength; i++)   txbuf.pktPayload[i] = _txBuffer[i];

    return (MAC_Send_Out ());
}


uint8 MAC_Send_Out (void)                                   // MAC 发送程序。不要直接调用
{
//  本程序为MAC发送程序，采用顺序控制，仅适合单一数据发送，未考虑多任务、并行处理。
//  本程序未使用中断，一般情况下中断对本程序并无影响。

    mac_stat = MAC_SENDING;                                 // MAC状态：发送中
    SX1211_Operating_Mode (SX1211_MODE_STDBY);              // 唤醒
    SX1211_FIFO_Standby (SX1211_STDBY_FIFO_WRITE);          // 使得STDBY模式FIFO用于写入发送

    while (MCU_IO_GET(HAL_SX1211_CLKOUT_PORT, HAL_SX1211_CLKOUT_PIN) == 0x00);                                          // 等待CLK信号！！！！！！！！！
    SX1211_TX_Bytes ((uint32)&txbuf, txbuf.packetLength+1); // 写入缓存
    SX1211_Operating_Mode (SX1211_MODE_TX);                 // TX模式

    while (mac_stat != MAC_SEND_OVER)   __low_power_mode_0();
    SX1211_Operating_Mode (SX1211_MODE_STDBY);          // 待机
    mac_stat = MAC_IDLE;                                // MAC状态：等待

    return (TRUE);                                      // 返回
}



uint8 MAC_Rx (void)                                 // 进入接收状态。接收到一个数据包后，自动进入接收状态
{
    if (mac_stat == MAC_ERROR)  return (FALSE);     // 如模块错误，退出
    SX1211_FIFO_Standby (SX1211_STDBY_FIFO_READ);       // 使得接收到数据后，进入STDBY模式也可收数
    SX1211_Operating_Mode (SX1211_MODE_RX);             // RX模式
    mac_stat = MAC_RECEIVING;                           // MAC状态：接收中
    return (TRUE);
}


uint8 MAC_Idle (void)                                   // 进入待机状态
{
    if (mac_stat == MAC_ERROR)  return (FALSE);     // 如模块错误，退出
    SX1211_Operating_Mode (SX1211_MODE_STDBY);          // 待机
    SX1211_FIFO_Standby (SX1211_STDBY_FIFO_WRITE);      // 使得STDBY模式FIFO用于写入发送
    mac_stat = MAC_IDLE;                                // MAC状态：无
    return (TRUE);
}


uint8 MAC_sleep (void)                                  // 进入睡眠状态
{
    if (mac_stat == MAC_ERROR)  return (FALSE);     // 如模块错误，退出
    SX1211_Operating_Mode (SX1211_MODE_SLEEP);          // 睡眠
    mac_stat = MAC_SLEEPING;                            // MAC状态：睡眠
    return (TRUE);
}



void IRQ0_isr (void)                                                    // IRQ0中断处理程序
{

//  IO_WAKE_INT = TRUE;                                                 // 退出DMA并唤醒CPU
}


void IRQ1_isr (void)                                                    // IRQ1中断处理程序
{
    if (SX1211_DATA_MODE == SX1211_DATA_MODE_IO)                        // IO模式：
    {


    }
    else                                                                // 字节或数据包模式：
    {
        if (SX1211_in_RX)                                               // 如接收态
        {
            if (SX1211_DATA_MODE == SX1211_DATA_MODE_PACKET)            // 收到CRC OK的数据包
            {
                SX1211_Operating_Mode (SX1211_MODE_STDBY);              // 稍息
                SX1211_RX_Bytes (&rxbuf.packetLength);                  // 接收数据
                mac_stat = MAC_RECV_DATA;                               // 置接收完毕标志
                SX1211_Operating_Mode (SX1211_MODE_RX);                 // 立正

                rxbuf.packetLength = rxbuf.packetLength - sizeof(macDataHead) - 1;  // 计算载荷大小
            }
            else
            {   ;   }                                                   // 字节模式暂无定义
        }
        else if (SX1211_in_TX)                                          // 如发送态
        {
            mac_stat = MAC_SEND_OVER;                                   // 置发送完毕标志
        }
    }

    IO_WAKE_INT = TRUE;                                                 // 退出DMA并唤醒CPU
}

