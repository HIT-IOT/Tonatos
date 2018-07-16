/*
    MAC Э��汾 v0.1

�������ڣ�2017.1.9
���Ĺ��ܣ���Ϊ��ԴMAC��ʼ��


    ���ӹ㲥ͨ�ţ����м�
    �����ǵ�����֡(�䳤)��δ�漰��֡����������չ
    δ����IOģʽ���ֽ�ģʽ

    ��ַΪ8λ������չΪ16λ

    �������յ�������ݰ�ʱ����������ٶȲ��������ж����Ŀ���
    ����������



    ����ʱ�������TX_DONE�ж��źţ���ʱ���һλ���ڷ����У�Ӧ��һ����ʱ���䷢����ϡ�


    mac_stat��MAC״̬
        0x00����
        0x01������
        0x02��������
        0x04��
        0x08��������
        0x10��������
        0x20�����յ�ACK
        0x40�����յ�ָ��
        0x80�����յ�����




ϵͳ������
        mac_stat��MAC����״̬


�ײ�ʹ��SX1211��IEAҲ��ʹ��CCϵ��


*/

#include <MAC_radio.h>
#include <SX1211.h>
#include <macros.h>
#include <defs.h>
#include <types.h>
#include <hal.h>
#include <msp430.h>

macDataPacket txbuf;                                // ���ͻ���
macDataPacket rxbuf;                                // ���ջ���


uint8 mac_stat = MAC_ERROR;                         // MAC״̬
uint8 rx_buff_size;                                 // ���ջ����С / ���յ����ֽڸ���



uint8 MAC_Init(void)                                // ��ʼ��MAC�����������˯��״̬
{
    if (initiate_SX1211() == FALSE)                 // ��ʼ��SX1211
    {                                               // �粻�ɹ���
        mac_stat = MAC_ERROR;                       // MAC״̬������
        return (FALSE);                             // ���ش���
    }

//  SX1211_Address_FILT (SX1211_ADDR_FILT_ON_0);    // �����ձ�����ַ��0x00
//  SX1211_Operating_Mode (SX1211_MODE_SLEEP);      // ˯��
    mac_stat = MAC_SLEEPING;                        // MAC״̬��˯��

    txbuf.pktHeader.via = LOCAL_ADDRESS;
    txbuf.pktHeader.stamp = 0x00;
    txbuf.pktHeader.nwkMode = 0x00;
    txbuf.ctrlByte = 0x00;

    return (TRUE);                                  // ��ʼ���ɹ�
}

void MAC_Off(void)                                  // �ر�MAC
{
    mac_stat = MAC_ERROR;                           // MAC״̬������
    SX1211_Operating_Mode(SX1211_MODE_SLEEP);      // ˯��
}



uint8 MAC_Send_Data (uint8 _dstAddress, uint8 _dataLength, uint8 *_txBuffer)    // �������ݣ�������Ϻ�������״̬������Ӧ��64�ֽ�����
{
    uint8 i;

    if (mac_stat == MAC_ERROR)  return (FALSE);             // ��ģ������˳�
    if (_dataLength > MAC_Payload_Length)   return (FALSE); // �����ƣ���ʱδ���Ƕ�֡

    txbuf.packetLength = sizeof(macDataHead) + 1 + _dataLength; // ����ͷ���� + ֡���ܱ�ʶ + �غɳ���
    txbuf.pktHeader.to = _dstAddress;
    txbuf.pktHeader.nwkMode = 0;                                // ��ʱ�����ǵ�֡
    txbuf.pktHeader.powerPercent = sys_status.Pri_voltage /30;  // ��3VΪ100%
    txbuf.ctrlByte = 0x00;

    for (i=0; i<_dataLength; i++)   txbuf.pktPayload[i] = _txBuffer[i];

    return (MAC_Send_Out ());
}


uint8 MAC_Send_Out (void)                                   // MAC ���ͳ��򡣲�Ҫֱ�ӵ���
{
//  ������ΪMAC���ͳ��򣬲���˳����ƣ����ʺϵ�һ���ݷ��ͣ�δ���Ƕ����񡢲��д���
//  ������δʹ���жϣ�һ��������ж϶Ա�������Ӱ�졣

    mac_stat = MAC_SENDING;                                 // MAC״̬��������
    SX1211_Operating_Mode (SX1211_MODE_STDBY);              // ����
    SX1211_FIFO_Standby (SX1211_STDBY_FIFO_WRITE);          // ʹ��STDBYģʽFIFO����д�뷢��

    while (MCU_IO_GET(HAL_SX1211_CLKOUT_PORT, HAL_SX1211_CLKOUT_PIN) == 0x00);                                          // �ȴ�CLK�źţ�����������������
    SX1211_TX_Bytes ((uint32)&txbuf, txbuf.packetLength+1); // д�뻺��
    SX1211_Operating_Mode (SX1211_MODE_TX);                 // TXģʽ

    while (mac_stat != MAC_SEND_OVER)   __low_power_mode_0();
    SX1211_Operating_Mode (SX1211_MODE_STDBY);          // ����
    mac_stat = MAC_IDLE;                                // MAC״̬���ȴ�

    return (TRUE);                                      // ����
}



uint8 MAC_Rx (void)                                 // �������״̬�����յ�һ�����ݰ����Զ��������״̬
{
    if (mac_stat == MAC_ERROR)  return (FALSE);     // ��ģ������˳�
    SX1211_FIFO_Standby (SX1211_STDBY_FIFO_READ);       // ʹ�ý��յ����ݺ󣬽���STDBYģʽҲ������
    SX1211_Operating_Mode (SX1211_MODE_RX);             // RXģʽ
    mac_stat = MAC_RECEIVING;                           // MAC״̬��������
    return (TRUE);
}


uint8 MAC_Idle (void)                                   // �������״̬
{
    if (mac_stat == MAC_ERROR)  return (FALSE);     // ��ģ������˳�
    SX1211_Operating_Mode (SX1211_MODE_STDBY);          // ����
    SX1211_FIFO_Standby (SX1211_STDBY_FIFO_WRITE);      // ʹ��STDBYģʽFIFO����д�뷢��
    mac_stat = MAC_IDLE;                                // MAC״̬����
    return (TRUE);
}


uint8 MAC_sleep (void)                                  // ����˯��״̬
{
    if (mac_stat == MAC_ERROR)  return (FALSE);     // ��ģ������˳�
    SX1211_Operating_Mode (SX1211_MODE_SLEEP);          // ˯��
    mac_stat = MAC_SLEEPING;                            // MAC״̬��˯��
    return (TRUE);
}



void IRQ0_isr (void)                                                    // IRQ0�жϴ������
{

//  IO_WAKE_INT = TRUE;                                                 // �˳�DMA������CPU
}


void IRQ1_isr (void)                                                    // IRQ1�жϴ������
{
    if (SX1211_DATA_MODE == SX1211_DATA_MODE_IO)                        // IOģʽ��
    {


    }
    else                                                                // �ֽڻ����ݰ�ģʽ��
    {
        if (SX1211_in_RX)                                               // �����̬
        {
            if (SX1211_DATA_MODE == SX1211_DATA_MODE_PACKET)            // �յ�CRC OK�����ݰ�
            {
                SX1211_Operating_Mode (SX1211_MODE_STDBY);              // ��Ϣ
                SX1211_RX_Bytes (&rxbuf.packetLength);                  // ��������
                mac_stat = MAC_RECV_DATA;                               // �ý�����ϱ�־
                SX1211_Operating_Mode (SX1211_MODE_RX);                 // ����

                rxbuf.packetLength = rxbuf.packetLength - sizeof(macDataHead) - 1;  // �����غɴ�С
            }
            else
            {   ;   }                                                   // �ֽ�ģʽ���޶���
        }
        else if (SX1211_in_TX)                                          // �緢��̬
        {
            mac_stat = MAC_SEND_OVER;                                   // �÷�����ϱ�־
        }
    }

    IO_WAKE_INT = TRUE;                                                 // �˳�DMA������CPU
}

