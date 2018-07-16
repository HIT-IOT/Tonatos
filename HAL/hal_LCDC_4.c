/*
���ܣ���ͨ4�ַ�Һ���� for FR6979
���ߣ�����
��д���ڣ�2016.12.9

��д���ڣ�2017.6.14
��д���ܣ�������Ч��־
--------------------------------------------------------------------

���ԡ�ע�⣺
���ܼ򻯣�����������ʾ���ܡ�δ������˸


�ӿڡ�����˵����

*/


// ������������ض��Ķ�ʽҺ��

#include <hal_LCDC_4.h>
#include <hal_LCD4_CODE.h>
#include <types.h>
#include <defs.h>
#include <msp430.h>
#include <macros.h>


static volatile uint8 *LCD_BUF;
static bool LCD_inactive_flag = TRUE;       // ʧЧ��־


void LCD_on (void)                              // ��ʽҺ����ʾ����һ������LCD����Ҫ�رգ�����
{
    LCD_BUF = &LCDM1;                           // ����LCD_BUF

    MCU_IO_PERIPHERAL_3 (6, 3, MCU_IO_DIR_OUTPUT);  // COM0
    MCU_IO_PERIPHERAL_3 (6, 4, MCU_IO_DIR_OUTPUT);  // COM1
    MCU_IO_PERIPHERAL_3 (6, 5, MCU_IO_DIR_OUTPUT);  // COM2
    MCU_IO_PERIPHERAL_3 (6, 6, MCU_IO_DIR_OUTPUT);  // COM3

    LCDCPCTL0_L = 0xFF;                         // S0-S7

    LCDCCTL0 = LCDDIV_7 + LCDPRE__32 + LCD4MUX + LCDLP; // 4mux LCD, 64Hz, LPmode
    LCDCMEMCTL |= LCDCLRM;                      // ����
    LCDCVCTL = LCDCPEN + VLCD_8;                // Charge pump enabled, reference from Internal. pump to 3.02V

    LCDCCTL0 |= LCDON;
    LCD_inactive_flag = FALSE;                  // ģ����Ч
}


void LCD_off (void)                         // ��ʽҺ����ʾ�ء�һ������LCD����Ҫ�رգ�����      ����Ӱ��VCC����Ҫ����AD��SX1211
{
    LCDCCTL0 = 0;                               // LCD off
    LCDCVCTL = 0;                               // Charge pump disabled

    LCD_inactive_flag = TRUE;                   // ģ��ʧЧ
}


void LCD_print (uint8 *ptr)                 // ��ʾ�ַ�
{
    uint8 i;
    if (LCD_inactive_flag == TRUE)  return;     // ģ��ʧЧ���˳�

    for (i=0; i<4; i++)
    {
        if (ptr[i] == ' ')      LCD_BUF[i] = _lcd_blank;
        else if ((ptr[i] >=0x30) && (ptr[i] <=0x39))        LCD_BUF[i] = char_gen[ptr[i] -0x30];
    }
}

void LCD_print_num (uint16 value)               // ��ʾ����
{
    uint8 i;

    if (LCD_inactive_flag == TRUE)  return;     // ģ��ʧЧ���˳�

    i = (value/1000) %10;                       // ǧλ
    if (i != 0)
    {
        LCD_BUF[0] = char_gen[i];               // ǧλ��Ϊ0

        i = (value/100) %10;
        LCD_BUF[1] = char_gen[i];               // ��λ
        i = (value/10) %10;
        LCD_BUF[2] = char_gen[i];               // ʮλ
        i = value %10;
        LCD_BUF[3] = char_gen[i];               // ��λ
    }
    else
    {
        LCD_BUF[0] = _lcd_blank;                    // ǧλΪ0

        i = (value/100) %10;                    // ��λ
        if (i != 0)
        {
            LCD_BUF[1] = char_gen[i];           // ��λ��Ϊ0

            i = (value/10) %10;
            LCD_BUF[2] = char_gen[i];           // ʮλ
            i = value %10;
            LCD_BUF[3] = char_gen[i];           // ��λ
        }
        else
        {
            LCD_BUF[1] = _lcd_blank;                // ��λΪ0

            i = (value/10) %10;                 // ʮλ
            if (i != 0)     LCD_BUF[2] = char_gen[i];
            else            LCD_BUF[2] = _lcd_blank;

            i = value %10;
            LCD_BUF[3] = char_gen[i];           // ��λ
        }
    }
}

void LCD_dot (uint8 dots)                       // ��ʾ�㣨���룺0-15��
{
    if (LCD_inactive_flag == TRUE)  return;     // ģ��ʧЧ���˳�

    if ((dots & BV(0)) !=0)     LCD_BUF[0] |= _lcd_dp;
    else                        LCD_BUF[0] &= ~_lcd_dp;

    if ((dots & BV(1)) !=0)     LCD_BUF[3] |= _lcd_dp;
    else                        LCD_BUF[3] &= ~_lcd_dp;

    if ((dots & BV(2)) !=0)     LCD_BUF[2] |= _lcd_dp;
    else                        LCD_BUF[2] &= ~_lcd_dp;

    if ((dots & BV(3)) !=0)     LCD_BUF[1] |= _lcd_dp;
    else                        LCD_BUF[1] &= ~_lcd_dp;
}
