#include <msp430.h>
#include <intrinsics.h>
#include <osal.h>
#include <hal.h>
#include <task.h>
#include <signal.h>
#include <defs.h>
#include <dma.h>
#include <diagnose.h>
#include <Energy_S&C.h>
// *************************    ϵͳ����    ************************************


void idle(void * param)
{
    while(1) {
        while (DMA_IN_USE (DMA_CHANNEL_2));             // �ȴ�DIAGͨ�����
           while (DMA_IN_USE (DMA_CHANNEL_0));             // �ȴ�MACͨ�����
           __low_power_mode_3();
    }
    while(1)
        __low_power_mode_3();
}

void task1(void * param)
{
    if (!second_flag)   return;
        second_flag = FALSE;
    Energy_Volt_Sense (Energy_Voltage_C1);                                              // ��ѹ��֪��C1
    Energy_Current_Sense (Energy_Sample_and_Clear_1st + Energy_Sample_and_Clear_2nd);   // ������֪����������������1��2������λ��ע��������1���ڵ���������Ч��

    sys_status.Pri_voltage = Energy_measure_result.Pri_cap_volt /1000;
    sys_status.Aux_voltage = Energy_measure_result.Sec_cap_volt /1000;


    LCD_print_num (sys_status.Pri_voltage);
    LCD_dot (0x08);

    if (sys_status.Pri_voltage <2200)       diag_power (0x01);
    else if (sys_status.Pri_voltage <2400)      diag_power (0x003);
    else if (sys_status.Pri_voltage <2600)      diag_power (0x007);
    else if (sys_status.Pri_voltage <2800)      diag_power (0x00F);
    else if (sys_status.Pri_voltage <3100)      diag_power (0x01F);
    else if (sys_status.Pri_voltage <3400)      diag_power (0x03F);
    else if (sys_status.Pri_voltage <3800)      diag_power (0x07F);
    else if (sys_status.Pri_voltage <4300)      diag_power (0x0FF);
    else if (sys_status.Pri_voltage <4900)      diag_power (0x1FF);
    else diag_power (0x3FF);


    diag_clrscr ();                 // ��0�п�ͷ

    uint32 Eharv, Econs;

    Eharv = Energy_measure_result.Harv_1st_charge;
    Econs = Energy_measure_result.Harv_2nd_charge;

    if (Eharv < 100000)
    {
        Eharv -= 75000;
        Eharv *= 4;
    }

    if (Econs < 75000)
    {
        Econs -= 70000;
        Econs *= 15;
    }

    Eharv = Eharv / 2000;       // ��ʵ��ȡ������΢��
    Econs = Econs / 1000;       // ��ʵ���ĵ�����΢��

    Eharv = (Eharv * sys_status.Pri_voltage) /1000;
    Econs = (Econs * sys_status.Pri_voltage) /1000;

    diagnose (" C1: %u mV.   \n", sys_status.Pri_voltage);
    diagnose (" C2: %u mV.   \n", sys_status.Aux_voltage);
    diagnose (" HARV: %l uJ.   \n", Eharv);
    diagnose (" CONS: %l uJ.   \n", Econs);
}

void main(void)
{
    sys_status.mode.status = 0x00;
    sys_status.power.status = 0x00;
    sys_status.mode.bits.INITIALIZING = TRUE;                   // �ó�ʼ����־
    hal_init();
    sys_status.mode.bits.INITIALIZING = FALSE;          // ���ʼ����־
    osal_init();

    task_create(idle, NULL);
    task_create(task1, NULL);
    TA0CCTL0 = CCIE;
    TA0CCR0 = 0x270f;   // 2.5ms time intervals with 4MHz
    TA0CTL = TASSEL_2 | ID_0 | TACLR | MC_1;
    _enable_interrupts();
    osal_start();
}
