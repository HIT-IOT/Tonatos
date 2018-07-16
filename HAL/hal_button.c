//  �����ж�

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
    if (halDigioConfig(&pin_button1) ==HAL_DIGIO_OK)                    // ���� button1 ����
    {
        halDigioIntSetEdge (&pin_button1, HAL_DIGIO_INT_FALLING_EDGE);  // �½��ش���
        halDigioIntConnect (&pin_button1, button1_INT_RSP);             // ��ʼ���жϣ���������
        halDigioIntClear (&pin_button1);                                // ���жϱ�־
        halDigioIntEnable (&pin_button1);                               // �����ж�
    }
/*
    if (halDigioConfig(&pin_button2) ==HAL_DIGIO_OK)                    // ���� button2 ����
    {
        halDigioIntSetEdge (&pin_button2, HAL_DIGIO_INT_FALLING_EDGE);  // �½��ش���
        halDigioIntConnect (&pin_button2, button2_INT_RSP);             // ��ʼ���жϣ���������
        halDigioIntClear (&pin_button2);                                // ���жϱ�־
        halDigioIntEnable (&pin_button2);                               // �����ж�
    }

    if (halDigioConfig(&pin_button3) ==HAL_DIGIO_OK)                    // ���� button3 ����
    {
        halDigioIntSetEdge (&pin_button3, HAL_DIGIO_INT_FALLING_EDGE);  // �½��ش���
        halDigioIntConnect (&pin_button3, button3_INT_RSP);             // ��ʼ���жϣ���������
        halDigioIntClear (&pin_button3);                                // ���жϱ�־
        halDigioIntEnable (&pin_button3);                               // �����ж�
    }

    if (halDigioConfig(&pin_button4) ==HAL_DIGIO_OK)                    // ���� button4 ����
    {
        halDigioIntSetEdge (&pin_button4, HAL_DIGIO_INT_FALLING_EDGE);  // �½��ش���
        halDigioIntConnect (&pin_button4, button4_INT_RSP);             // ��ʼ���жϣ���������
        halDigioIntClear (&pin_button4);                                // ���жϱ�־
        halDigioIntEnable (&pin_button4);                               // �����ж�
    }
*/
}

void button1_INT_RSP (void)                             // button1 �ж���Ӧ
{
/*
    C2_Charge_EN (ddd);                                 // �򳬼����ݳ�磬10�����ѿɼ���ѹ����
    Charge_pump_EN(ddd);
    LCD_print_num (ddd);

    ddd++;
    if (ddd > 15)   ddd = 0;
*/
//  rtc_wake (20000);                                       // ���ڻ���60��

/*
        test[0] = HI_UINT16(sys_counter);
        test[1] = LO_UINT16(sys_counter);

        MAC_Send_Data (0x11, 2, test);
//      MAC_Send_Data (0x12, 2, test);
//      MAC_Send_Data (0x13, 2, test);
//      MAC_Send_Data (0x14, 2, test);                          ��������������
//      MAC_Send_Data (0x15, 2, test);
        MAC_sleep ();
*/
        led_tgl ();

//  IO_WAKE_INT = TRUE;                                                 // �˳�DMA������CPU
}
