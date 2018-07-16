
#ifndef HAL_LCDA_H
#define HAL_LCDA_H
#include <types.h>

#define LCD_clr()   st( LCDCMEMCTL |= LCDCLRM; )        // ����


extern void LCD_on (void);                              // Һ����ʾ����һ������LCD����Ҫ�رգ�����
extern void LCD_off (void);                         // Һ����ʾ�ء�һ������LCD����Ҫ�رգ�����

extern void LCD_print (uint8 *ptr);                 // ��ʾ�ַ�
extern void LCD_print_num (uint16 value);               // ��ʾ����
extern void LCD_dot (uint8 dots);                       // ��ʾ�㣨���룺0-15��

#endif
