//  按键中断

/***********************************************************************************
* INCLUDES
*/
#include <hal_button.h>
#include <hal_digio.h>
#include <msp430.h>

const digioConfig pin_button1 = {4, 4, BIT4, HAL_DIGIO_INPUT,  0};      // button1
//const digioConfig pin_button2 = {1, 6, BIT6, HAL_DIGIO_INPUT,  0};    // button2
//const digioConfig pin_button3 = {1, 0, BIT0, HAL_DIGIO_INPUT,  0};    // button3
//const digioConfig pin_button4 = {1, 0, BIT0, HAL_DIGIO_INPUT,  0};    // button3

void init_button (void)
{
    if (halDigioConfig(&pin_button1) ==HAL_DIGIO_OK)                    // 配置 button1 输入
    {
        halDigioIntSetEdge (&pin_button1, HAL_DIGIO_INT_FALLING_EDGE);  // 下降沿触发
        halDigioIntConnect (&pin_button1, button1_INT_RSP);             // 初始化中断，建立连接
        halDigioIntClear (&pin_button1);                                // 清中断标志
        halDigioIntEnable (&pin_button1);                               // 开启中断
    }
/*
    if (halDigioConfig(&pin_button2) ==HAL_DIGIO_OK)                    // 配置 button2 输入
    {
        halDigioIntSetEdge (&pin_button2, HAL_DIGIO_INT_FALLING_EDGE);  // 下降沿触发
        halDigioIntConnect (&pin_button2, button2_INT_RSP);             // 初始化中断，建立连接
        halDigioIntClear (&pin_button2);                                // 清中断标志
        halDigioIntEnable (&pin_button2);                               // 开启中断
    }

    if (halDigioConfig(&pin_button3) ==HAL_DIGIO_OK)                    // 配置 button3 输入
    {
        halDigioIntSetEdge (&pin_button3, HAL_DIGIO_INT_FALLING_EDGE);  // 下降沿触发
        halDigioIntConnect (&pin_button3, button3_INT_RSP);             // 初始化中断，建立连接
        halDigioIntClear (&pin_button3);                                // 清中断标志
        halDigioIntEnable (&pin_button3);                               // 开启中断
    }

    if (halDigioConfig(&pin_button4) ==HAL_DIGIO_OK)                    // 配置 button4 输入
    {
        halDigioIntSetEdge (&pin_button4, HAL_DIGIO_INT_FALLING_EDGE);  // 下降沿触发
        halDigioIntConnect (&pin_button4, button4_INT_RSP);             // 初始化中断，建立连接
        halDigioIntClear (&pin_button4);                                // 清中断标志
        halDigioIntEnable (&pin_button4);                               // 开启中断
    }
*/
}

void button1_INT_RSP (void)                             // button1 中断响应
{
/*
    C2_Charge_EN (ddd);                                 // 向超级电容充电，10左右已可见电压升高
    Charge_pump_EN(ddd);
    LCD_print_num (ddd);

    ddd++;
    if (ddd > 15)   ddd = 0;
*/
//  rtc_wake (20000);                                       // 周期唤醒60秒

/*
        test[0] = HI_UINT16(sys_counter);
        test[1] = LO_UINT16(sys_counter);

        MAC_Send_Data (0x11, 2, test);
//      MAC_Send_Data (0x12, 2, test);
//      MAC_Send_Data (0x13, 2, test);
//      MAC_Send_Data (0x14, 2, test);                          连发！！！！！
//      MAC_Send_Data (0x15, 2, test);
        MAC_sleep ();
*/
        led_tgl ();

//  IO_WAKE_INT = TRUE;                                                 // 退出DMA并唤醒CPU
}
