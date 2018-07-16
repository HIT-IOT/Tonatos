/*
功能：普通4字符液晶屏 for FR6979
作者：张扬
改写日期：2016.12.9

改写日期：2017.6.14
改写功能：增加有效标志
--------------------------------------------------------------------

特性、注意：
功能简化，仅有数字显示功能。未开启闪烁


接口、函数说明：

*/


// 本程序仅用于特定的段式液晶

#include <hal_LCDC_4.h>
#include <hal_LCD4_CODE.h>
#include <types.h>
#include <defs.h>
#include <msp430.h>
#include <macros.h>


static volatile uint8 *LCD_BUF;
static bool LCD_inactive_flag = TRUE;       // 失效标志


void LCD_on (void)                              // 段式液晶显示开。一旦开启LCD，不要关闭！！！
{
    LCD_BUF = &LCDM1;                           // 定义LCD_BUF

    MCU_IO_PERIPHERAL_3 (6, 3, MCU_IO_DIR_OUTPUT);  // COM0
    MCU_IO_PERIPHERAL_3 (6, 4, MCU_IO_DIR_OUTPUT);  // COM1
    MCU_IO_PERIPHERAL_3 (6, 5, MCU_IO_DIR_OUTPUT);  // COM2
    MCU_IO_PERIPHERAL_3 (6, 6, MCU_IO_DIR_OUTPUT);  // COM3

    LCDCPCTL0_L = 0xFF;                         // S0-S7

    LCDCCTL0 = LCDDIV_7 + LCDPRE__32 + LCD4MUX + LCDLP; // 4mux LCD, 64Hz, LPmode
    LCDCMEMCTL |= LCDCLRM;                      // 清屏
    LCDCVCTL = LCDCPEN + VLCD_8;                // Charge pump enabled, reference from Internal. pump to 3.02V

    LCDCCTL0 |= LCDON;
    LCD_inactive_flag = FALSE;                  // 模块有效
}


void LCD_off (void)                         // 段式液晶显示关。一旦开启LCD，不要关闭！！！      否则影响VCC，主要干扰AD和SX1211
{
    LCDCCTL0 = 0;                               // LCD off
    LCDCVCTL = 0;                               // Charge pump disabled

    LCD_inactive_flag = TRUE;                   // 模块失效
}


void LCD_print (uint8 *ptr)                 // 显示字符
{
    uint8 i;
    if (LCD_inactive_flag == TRUE)  return;     // 模块失效，退出

    for (i=0; i<4; i++)
    {
        if (ptr[i] == ' ')      LCD_BUF[i] = _lcd_blank;
        else if ((ptr[i] >=0x30) && (ptr[i] <=0x39))        LCD_BUF[i] = char_gen[ptr[i] -0x30];
    }
}

void LCD_print_num (uint16 value)               // 显示数字
{
    uint8 i;

    if (LCD_inactive_flag == TRUE)  return;     // 模块失效，退出

    i = (value/1000) %10;                       // 千位
    if (i != 0)
    {
        LCD_BUF[0] = char_gen[i];               // 千位不为0

        i = (value/100) %10;
        LCD_BUF[1] = char_gen[i];               // 百位
        i = (value/10) %10;
        LCD_BUF[2] = char_gen[i];               // 十位
        i = value %10;
        LCD_BUF[3] = char_gen[i];               // 个位
    }
    else
    {
        LCD_BUF[0] = _lcd_blank;                    // 千位为0

        i = (value/100) %10;                    // 百位
        if (i != 0)
        {
            LCD_BUF[1] = char_gen[i];           // 百位不为0

            i = (value/10) %10;
            LCD_BUF[2] = char_gen[i];           // 十位
            i = value %10;
            LCD_BUF[3] = char_gen[i];           // 个位
        }
        else
        {
            LCD_BUF[1] = _lcd_blank;                // 百位为0

            i = (value/10) %10;                 // 十位
            if (i != 0)     LCD_BUF[2] = char_gen[i];
            else            LCD_BUF[2] = _lcd_blank;

            i = value %10;
            LCD_BUF[3] = char_gen[i];           // 个位
        }
    }
}

void LCD_dot (uint8 dots)                       // 显示点（输入：0-15）
{
    if (LCD_inactive_flag == TRUE)  return;     // 模块失效，退出

    if ((dots & BV(0)) !=0)     LCD_BUF[0] |= _lcd_dp;
    else                        LCD_BUF[0] &= ~_lcd_dp;

    if ((dots & BV(1)) !=0)     LCD_BUF[3] |= _lcd_dp;
    else                        LCD_BUF[3] &= ~_lcd_dp;

    if ((dots & BV(2)) !=0)     LCD_BUF[2] |= _lcd_dp;
    else                        LCD_BUF[2] &= ~_lcd_dp;

    if ((dots & BV(3)) !=0)     LCD_BUF[1] |= _lcd_dp;
    else                        LCD_BUF[1] &= ~_lcd_dp;
}
