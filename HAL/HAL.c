/*
 * hal_init.c
 *
 *  Created on: 2018��4��26��
 *      Author: carpela
 */
#include <bsp_dma.h>
#include <bsp_timer_a.h>
#include <diagnose.h>
#include <hal.h>
#include <msp430.h>
#include <RTC.h>
#include <mac_radio.h>
#include <Energy_S&C.h>
#include <hal_button.h>
#include <hal_digio.h>
#include <macros.h>
#include <hal_led.h>
#include <hal_LCDC_4.h>
#include <hal_beeper.h>

void SYNC_isr(void);
SysStatus sys_status;
uint32 HAL_CPU_CLOCK_Hz;                    // ��Ƶ. ��λ��Hz
uint16 HAL_CPU_CLOCK_KHz;                   // ��Ƶ. ��λ��KHz. up to 65M
uint16 HAL_CPU_CLOCK_DELAY;                 // ��ʱ����ר�ã��������
bool sync_flag;                             // ͬ����־
void POA_INT_RSP (void) ;

const digioConfig Pwr_Off_Alert = {HAL_PWR_ALERT_PORT, HAL_PWR_ALERT_PIN, HAL_PWR_ALERT_PIN_BIT, HAL_DIGIO_INPUT,  0};      // ����Ԥ��

void hal_init(void)
{
    /*  Disable Watchdog    */
    WDTCTL = WDTPW + WDTHOLD;

    /*  Digital IO Initialization   */
    P1SEL0 |= BIT2 + BIT3;
    P1SEL1 |= BIT2 + BIT3;
    P1OUT = 0;
    P1DIR = BIT4 + BIT5 + BIT6 + BIT7;

    P2OUT = 0;
    P2DIR = BIT0 + BIT2 + BIT4 + BIT5 + BIT6 + BIT7;

    P3OUT = 0;
    P3DIR = BIT0 + BIT3 + BIT4 + BIT6 + BIT7;

    P4OUT = 0;
    P4DIR = BIT0 + BIT1 + BIT5 + BIT6 + BIT7;

    P6OUT = 0;
    P6DIR = BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7;

    P7OUT = BIT0;
    P7DIR = BIT0 + BIT5 + BIT6 + BIT7;

    P9SEL0 |= BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7;
    P9SEL1 |= BIT0 + BIT1 + BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7;

    PJSEL0 = BIT4;
    PJOUT = 0;
    PJDIR = BIT0 + BIT1 + BIT2 + BIT3 + BIT6 + BIT7;
//    SYSCTL = SYSJTAGPIN;                                    // Dedicated JTAG pins

    /*  Unlock  GPIO    */
    PM5CTL0 &= ~LOCKLPM5;

    //  RTC.        Ԥ���硣RX6110оƬ���Ѻ�����û��һ���ʱ���ὫIIC���ͣ����¶����ܺġ�������Ѻ���RX6110����Ҫ���ٹ���һ�Ρ�
    rtc_power_on ();                                        // ����RTC���硣40ms�󷽿ɲ�����������Ҫɾ����

    //  Power off Alert
    if (halDigioConfig(&Pwr_Off_Alert) ==HAL_DIGIO_OK)      // ���� ����Ԥ��
    {
        halDigioIntSetEdge (&Pwr_Off_Alert, HAL_DIGIO_INT_RISING_EDGE); // �����ش���
        halDigioIntConnect (&Pwr_Off_Alert, POA_INT_RSP);               // ��ʼ���жϣ���������
        halDigioIntClear (&Pwr_Off_Alert);                              // ���жϱ�־
        halDigioIntEnable (&Pwr_Off_Alert);                             // �����ж�
    }

    led_off();
    /*  GIE skipped */
    _enable_interrupts();
    /*  PMM & SVS   */
    PMMCTL0_H = PMMPW_H;                                    // Open PMM registers for write access
    PMMCTL0_L = 0x00;                                       // SVS OFF
    PMMCTL0_H = 0x00;                                       // Lock PMM registers for write access
    /*  Clock System    */
    PJSEL1 &= ~BIT4;
    PJSEL0 |= BIT4;                                         // Port select XT1
    CSCTL0_H = CSKEY_H;                                     // ����CSģ��

    Set_Freq (HAL_CPU_MCLK_4M);                             // ��Ƶ4M

    CSCTL2 = SELA__VLOCLK + SELS__DCOCLK + SELM__DCOCLK;    // ACLK =VLO    MCLK =SMCLK =DCO
    CSCTL3 = DIVA__1 + DIVS__1 + DIVM__1;                   // ACLK/1   SMCLK/1     MCLK/1

    CSCTL4 = HFXTOFF + LFXTDRIVE_1;                         // XT2�رա�   XT1��������Դ��������1����VLO on, SMCLK on

    init_diag ();                                           // ��ϳ�ʼ��
    diag_clrscr ();                                         // ����
    diagnose ("MCLK: %uKHz.\n", HAL_CPU_CLOCK_KHz);         // ��ʾ��Ƶ, KHz

    if (MAC_Init ())    diagnose ("MAC Successful.\n");
    else    diagnose ("MAC Failure.\n");

    init_button();                                         // ���������ж�

    //  Energy      ��ҪLF
    Energy_Volt_init ();                                    // ����-��ѹ ��ʼ��
    Energy_Current_init ();                                 // ����-���� ��ʼ��

    while (DMA_IN_USE (DMA_CHANNEL_2));                     // �ȴ�DIAGͨ�����
    while (DMA_IN_USE (DMA_CHANNEL_0));                     // �ȴ�MACͨ�����        ��Լ0.5ms

    // -----------------    ��ִ��12ms����Ȼʣ��Լ188ms      --------------------
    TA3CCR0 = 1500;                                         // 160 ~ 200ms
    TA3CTL = TASSEL_1 + ID_0 + MC_1 + TACLR +  TAIE;        // VLO/1, up mode, interrupt enabled
    __low_power_mode_3();                                   // LPM3
    TA3CTL = 0;

    // -----------------    ʣ��Լ8ms����⾧������״̬ ------------------------
    do
    {
        CSCTL5 &= ~LFXTOFFG;                // Clear XT1 fault flag
        SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1 & OFIFG);              // Test oscillator fault flag

    CSCTL2 = SELA__LFXTCLK + SELS__DCOCLK + SELM__DCOCLK;   // ACLK =LFXT   MCLK =SMCLK =DCO
    CSCTL4 = HFXTOFF + LFXTDRIVE_0 + VLOOFF;                // XT2�رա�   XT1��������Դ��������0����VLO off, SMCLK on
    CSCTL0_H = 0;                                           // ����
    //  MCU_IO_PERIPHERAL_1 (J, 2, MCU_IO_DIR_OUTPUT);          // ACLK
    //  MCU_IO_PERIPHERAL_3 (7, 4, MCU_IO_DIR_OUTPUT);          // MCLK

    // *************************    ��Ƶ��������  ****************************************

    //  ��ѹ��     ��ҪLF
    Charge_pump_EN(CHARGE_PUMP_CLK_512);                    // ��ѹ�� 512Hz
    //  LCD_C       ��ҪLF        һ������LCD����Ҫ�رգ�����
    LCD_on ();                                              // ��ʽҺ����ʾ����һ������LCD����Ҫ�رգ�����


    //  RTC.    RX6110��Ҫ40ms��������ʱ�䣬�˳�ʼ��������ں��档      ��ҪDCO + LF
    SystemClockinit();                                      // ����ϵͳʱ��
    sync_flag = FALSE;

    digioConfig pinRTC_IRQ = {HAL_BOARD_RTC_INT_PORT, HAL_BOARD_RTC_INT_PIN, HAL_BOARD_RTC_INT_PIN_BIT, HAL_DIGIO_INPUT,  0};       // RTC INT
    uint16 rtc_status;

    rtc_status = rtc_valid_check ();
    if (rtc_status == HAL_BOARD_RX6110_READY)               // RTC���������ٳ�ʼ����ֱ�Ӷ�ȡʱ�䣬ͬ��
    {
//      rtc_read ();                                        // ��ȡʵʱʱ��
        rtc_sync_en ();                                     // ��IRQ2�����ͬ��ʱ�ӡ�һ���ڿ��ж�ǰ��ͬ��������

        halDigioIntSetEdge (&pinRTC_IRQ, HAL_DIGIO_INT_FALLING_EDGE);   // pinRTC_IRQ�½��ش���
        halDigioIntConnect (&pinRTC_IRQ, SYNC_isr);                     // ��ʼ���жϣ���������
        halDigioIntClear (&pinRTC_IRQ);                                 // ���жϱ�־
        halDigioIntEnable (&pinRTC_IRQ);                                // ����ͬ���ж�
    }
    else if (rtc_status == HAL_BOARD_RX6110_VLF_FAIL)       // RTC���磬���³�ʼ����
    {
        // ��1��
        // ��ͨ������ͬ��
        // ��ʱӦͨ������ͬ��ʱ�䣬ͬ������sync_flag = FALSE;ִ��rtc_preset ();������һ��ͬ����ִ��rtc_sync ();

        rtc_preset ();                                      // ����ʵʱʱ�ӡ�ע���������ϵ�δ��ͬ�������´��ϵ��ͬ����Ϣ�Ǵ���ġ�
        // rtc_sync ();                                     // ͬ��RTCʱ�ӡ�ִ�б�ָ���IRQ2�����ص��͵�λ
    }
    else                                                    // RTC�𻵻򲻴���
    {
        // ��δ����
    }

    //  rtc_power_off ();
}


void Set_Freq (uint8 level)                         // �趨��Ƶ����λ��0-6�� ��ʱֻ֧�ֵ�8M
{
    /*
    000b = If DCORSEL = 0: 1 MHz;       If DCORSEL = 1: 1 MHz
    001b = If DCORSEL = 0: 2.67 MHz;    If DCORSEL = 1: 5.33 MHz
    010b = If DCORSEL = 0: 3.5 MHz;     If DCORSEL = 1: 7 MHz
    011b = If DCORSEL = 0: 4 MHz;       If DCORSEL = 1: 8 MHz
    100b = If DCORSEL = 0: 5.33 MHz;    If DCORSEL = 1: 16 MHz
    101b = If DCORSEL = 0: 7 MHz;       If DCORSEL = 1: 21 MHz
    110b = If DCORSEL = 0: 8 MHz;       If DCORSEL = 1: 24 MHz
    */

    if (level > 6)  level = 6;

    switch (level)
    {
        case 0:
            HAL_CPU_CLOCK_KHz = 1002;
            HAL_CPU_CLOCK_DELAY = 166;
            break;
        case 1:
            HAL_CPU_CLOCK_KHz = 2660;
            HAL_CPU_CLOCK_DELAY = 445;
            break;
        case 2:
            HAL_CPU_CLOCK_KHz = 3490;
            HAL_CPU_CLOCK_DELAY = 583;
            break;
        case 3:
            HAL_CPU_CLOCK_KHz = 4008;
            HAL_CPU_CLOCK_DELAY = 666;
            break;
        case 4:
            HAL_CPU_CLOCK_KHz = 5314;
            HAL_CPU_CLOCK_DELAY = 888;
            break;
        case 5:
            HAL_CPU_CLOCK_KHz = 6985;
            HAL_CPU_CLOCK_DELAY = 1166;
            break;
        case 6:
            HAL_CPU_CLOCK_KHz = 8020;
            HAL_CPU_CLOCK_DELAY = 1333;
            break;
    }


    CSCTL1_L = level <<1;                       // ����Ƶ�ʣ����٣���ߵ�8M

}

void Charge_pump_EN (uint16 value)                      // ��ɱÿ�����ȡֵ0-7��Խ��Ƶ��Խ�ߡ�0Ϊ�ر�
{
    MCU_IO_PERIPHERAL_1 (HAL_CRG_PUMP_CLK_PORT, HAL_CRG_PUMP_CLK_PIN, MCU_IO_DIR_OUTPUT);   // TA0.1��    PWM: SEL_1

    if (value > CHARGE_PUMP_CLK_16K)    value = CHARGE_PUMP_CLK_16K;

    switch (value)
    {
        case CHARGE_PUMP_CLK_16K:   value = 2;      break;  // 32K/2 = 16K
        case CHARGE_PUMP_CLK_8K:    value = 4;      break;  // 32K/4 = 8K
        case CHARGE_PUMP_CLK_4K:    value = 8;      break;  // 32K/8 = 4K
        case CHARGE_PUMP_CLK_2K:    value = 16;     break;  // 32K/16 = 2K
        case CHARGE_PUMP_CLK_1K:    value = 32;     break;  // 32K/32 = 1K
        case CHARGE_PUMP_CLK_512:   value = 64;     break;  // 32K/64 = 512Hz
        case CHARGE_PUMP_CLK_256:   value = 128;    break;  // 32K/128 = 256Hz
        case CHARGE_PUMP_CLK_0:     value = 0;      break;  // 0Hz
    }

    uint16 duty;
    duty = value /2;                                        // 50%ռ�ձ�
    if (value != 0)     value--;

    TIMER_A_generatePWM (HAL_CRG_PUMP_PWM_BASE,
                         TIMER_A_CLOCKSOURCE_ACLK,          // ACLK, 32K
                         TIMER_A_CLOCKSOURCE_DIVIDER_1,     // ACLK/1
                         value,                             // ����
                         HAL_CRG_PUMP_PWM_CNL,
                         TIMER_A_OUTPUTMODE_SET_RESET,      // �͵�ƽ����
                         duty                               // 50%
                         );

    TIMER_A_startCounter (HAL_CRG_PUMP_PWM_BASE,
                          TIMER_A_UP_MODE                   // UP ģʽ����
                          );
}

void POA_INT_RSP (void)                             // ����Ԥ�� �ж���Ӧ
{
//  �����ѹ�����𵴣������źſ��ܲ�ֹһ����
//  ������ʱ�����ѹ�½�����Ҳ������õ����źš�

    beep(14);                                           // 5ms
}

void Charge_pump_DIS (void)                         // ��ɱ�ֹͣ
{
    Timer_A_stop (HAL_CRG_PUMP_PWM_BASE);                   // Timer STOP
}

void SYNC_isr(void)
{
    SystemClockSync();                                      // ��ͬ��
    sync_flag = TRUE;

    digioConfig pinRTC_IRQ = {HAL_BOARD_RTC_INT_PORT, HAL_BOARD_RTC_INT_PIN, HAL_BOARD_RTC_INT_PIN_BIT, HAL_DIGIO_INPUT,  0};       // RTC INT
    halDigioIntDisable (&pinRTC_IRQ);                       // �ر�ͬ���ж�
    rtc_sync_dis ();                                        // ��IRQ2�Ϲر�ͬ��ʱ��
}

uint32 sys_counter, sys_timer;                              // 32λ��������1���ʱ��
bool second_flag;

HAL_ISR_FUNCTION(Timer_A3,TIMER3_A1)
{
    uint16 i;
    TA3CTL &= ~TAIFG;

    sys_counter++;

#ifdef SYS_SLEEP_PERIOD                                     // ��˯��ʱ����ڵ���1��
    for (i=0; i<SYS_SLEEP_PERIOD; i++)
    {   sys_timer++;
        rtc_run ();     }                                   // ��ʱϵͳʱ�ӣ��������
    __low_power_mode_off_on_exit();                         // Clear CPUOFF bit from 0(SR)
#else
    if ((counter%SYS_SLEEP_FREQ) == 0)
    {
        sys_timer++;
        rtc_run ();
        __low_power_mode_off_on_exit();                     // Clear CPUOFF bit from 0(SR)
    }                                                       // ��μ���1��
#endif

    second_flag = TRUE;
}
