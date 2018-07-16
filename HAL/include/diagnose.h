#ifndef HAL_DIAG_H
#define HAL_DIAG_H

#include <types.h>

#define HAL_BOARD_DIAG_BASE             EUSCI_A1_BASE

#define HAL_BOARD_DIAG_STE_PORT         3           // 诊断使能
#define HAL_BOARD_DIAG_STE_PIN          7           // 诊断使能

#define HAL_BOARD_DIAG_SCK_PORT         3           // 诊断时钟线
#define HAL_BOARD_DIAG_SCK_PIN          6           // 诊断时钟线

#define HAL_BOARD_DIAG_MOSI_PORT        3           // 诊断数据线
#define HAL_BOARD_DIAG_MOSI_PIN         4           // 诊断数据线

#define HAL_BOARD_DIAG_MISO_PORT        3           // 诊断接收
#define HAL_BOARD_DIAG_MISO_PIN         5           // 诊断接收

#define HAL_BOARD_DIAG_BUF_LENGTH           160     // 发送缓存160字节

void init_diag (void);                              // 诊断初始化（需在晶振初始化前调用）
extern void diagnose (const uint8 *ptr,...);        // 发送诊断信息
extern void diag_cur (uint8 cur_y, uint8 cur_x);    // 显示坐标设定
extern void diag_clrscr (void);                 // 诊断清屏
extern void diag_power (uint16 _level);         // 电量显示
extern uint8 diag_busy (void);                      // 检测DIAG是否忙
void diag_sendout (void);                           // 发送缓存数据
void DMA2_diag_isr (void);                          // DMA发送中断处理程序

#endif
