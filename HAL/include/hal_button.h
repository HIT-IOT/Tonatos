#ifndef HAL_BUTTON_H
#define HAL_BUTTON_H

void init_button (void);                            // 按键初始化
void button1_INT_RSP (void);                        // button1 中断响应
void button2_INT_RSP (void);                        // button2 中断响应
void button3_INT_RSP (void);                        // button3 中断响应
void button4_INT_RSP (void);                        // button4 中断响应

#endif
