#ifndef MAC_BASICRF_H
#define MAC_BASICRF_H

#include <types.h>

#define     send            0
#define     reply           1

#define     MAC_Payload_Length          64          // ��֡�غɳ���


#define     MAC_RETX_CNT                3           // �ط�����



#define     LOCAL_ADDRESS               0x01        // ������ַ


/*******************************************************************************
    MAC״̬           */
#define     MAC_IDLE                    0x00            // ��
#define     MAC_ERROR                   0x01            // ����
#define     MAC_SLEEPING                0x02            // ������
#define     MAC_SENDING                 0x04            // ������
#define     MAC_SEND_OVER               0x08            // �������
#define     MAC_RECEIVING               0x10            // ������
#define     MAC_RECV_ACK                0x20            // ���յ�ACK
#define     MAC_RECV_CMD                0x40            // ���յ�ָ��
#define     MAC_RECV_DATA               0x80            // ���յ�����


/*******************************************************************************
    ����֡ ����ͷ                     �������ݽṹ���ܵ����ڴ��ַ������������
*/
typedef struct
{
uint8 to;                               // �����͸�..
uint8 from;                             // ����Դ��..
uint8 via;                              // ���ݾ���..
uint8 stamp;                            // �ʴ�
uint8 nwkMode;                          // ����ṹ��Ĭ�Ϲ㲥��0��
uint8 powerPercent;                     // ��������
//  uint8 commPower;                        // ͨ�Ź���
//  uint8 RSSI;                             // ͨ������ ��������Է���
} macDataHead;


/*******************************************************************************
    ����֡ ���ݣ���֡��
*/
typedef struct
{
uint8 packetLength;                     // ���ͣ����ݰ��ܳ��ȣ�������������
                                        // ���գ��غ��ֽ���
macDataHead pktHeader;                  // ����ͷ��֡��Ϣ��
uint8 ctrlByte;                         // ֡���ܱ�ʶ
uint8 pktPayload[MAC_Payload_Length];   // �غ�               // �˴��ɿ��Ǹ�Ϊ����ָ��
} macDataPacket;


/*******************************************************************************
    ��������
*/

extern uint8 mac_stat;                  // MAC״̬
extern macDataPacket txbuf;             // ���ͻ���
extern macDataPacket rxbuf;             // ���ջ���



/*******************************************************************************
    ��������
*/

extern uint8 MAC_Init(void);                                        // ��ʼ��MAC�����������˯��״̬
extern void MAC_Off(void);                                      // �ر�MAC
extern uint8 MAC_Send_Data (uint8 _dstAddress, uint8 _dataLength, uint8 *_txBuffer);    // �������ݣ�������Ϻ�������״̬������Ӧ��64�ֽ�����
extern uint8 MAC_Send_Out (void);                                   // MAC ���ͳ��򡣲�Ҫֱ�ӵ���
extern uint8 MAC_Rx (void);                                     // �������״̬�����յ�һ�����ݰ����Զ��������״̬
extern uint8 MAC_Idle (void);                                       // �������״̬
extern uint8 MAC_sleep (void);                                  // ����˯��״̬



extern void IRQ0_isr (void);                                            // IRQ0�жϴ������
extern void IRQ1_isr (void);                                            // IRQ1�жϴ������

#endif
