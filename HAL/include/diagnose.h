#ifndef HAL_DIAG_H
#define HAL_DIAG_H

#include <types.h>

#define HAL_BOARD_DIAG_BASE             EUSCI_A1_BASE

#define HAL_BOARD_DIAG_STE_PORT         3           // ���ʹ��
#define HAL_BOARD_DIAG_STE_PIN          7           // ���ʹ��

#define HAL_BOARD_DIAG_SCK_PORT         3           // ���ʱ����
#define HAL_BOARD_DIAG_SCK_PIN          6           // ���ʱ����

#define HAL_BOARD_DIAG_MOSI_PORT        3           // ���������
#define HAL_BOARD_DIAG_MOSI_PIN         4           // ���������

#define HAL_BOARD_DIAG_MISO_PORT        3           // ��Ͻ���
#define HAL_BOARD_DIAG_MISO_PIN         5           // ��Ͻ���

#define HAL_BOARD_DIAG_BUF_LENGTH           160     // ���ͻ���160�ֽ�

void init_diag (void);                              // ��ϳ�ʼ�������ھ����ʼ��ǰ���ã�
extern void diagnose (const uint8 *ptr,...);        // ���������Ϣ
extern void diag_cur (uint8 cur_y, uint8 cur_x);    // ��ʾ�����趨
extern void diag_clrscr (void);                 // �������
extern void diag_power (uint16 _level);         // ������ʾ
extern uint8 diag_busy (void);                      // ���DIAG�Ƿ�æ
void diag_sendout (void);                           // ���ͻ�������
void DMA2_diag_isr (void);                          // DMA�����жϴ������

#endif
