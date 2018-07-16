
#ifndef HAL_LCDA_H
#define HAL_LCDA_H
#include <types.h>

#define LCD_clr()   st( LCDCMEMCTL |= LCDCLRM; )        // 清屏


extern void LCD_on (void);                              // 液晶显示开。一旦开启LCD，不要关闭！！！
extern void LCD_off (void);                         // 液晶显示关。一旦开启LCD，不要关闭！！！

extern void LCD_print (uint8 *ptr);                 // 显示字符
extern void LCD_print_num (uint16 value);               // 显示数字
extern void LCD_dot (uint8 dots);                       // 显示点（输入：0-15）

#endif
