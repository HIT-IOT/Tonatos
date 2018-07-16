/*
 * hal_init.c
 *
 *  Created on: 2018年4月26日
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
uint32 HAL_CPU_CLOCK_Hz;                    // 主频. 单位：Hz
uint16 HAL_CPU_CLOCK_KHz;                   // 主频. 单位：KHz. up to 65M
uint16 HAL_CPU_CLOCK_DELAY;                 // 延时函数专用，不必理睬
bool sync_flag;                             // 同步标志
void POA_INT_RSP (void) ;

const digioConfig Pwr_Off_Alert = {HAL_PWR_ALERT_PORT, HAL_PWR_ALERT_PIN, HAL_PWR_ALERT_PIN_BIT, HAL_DIGIO_INPUT,  0};      // 掉电预警

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

    //  RTC.        预供电。RX6110芯片若已焊接且没有一点电时，会将IIC拉低，导致额外能耗。因此若已焊接RX6110，需要至少供电一次。
    rtc_power_on ();                                        // 开启RTC供电。40ms后方可操作。此条不要删除！

    //  Power off Alert
    if (halDigioConfig(&Pwr_Off_Alert) ==HAL_DIGIO_OK)      // 配置 掉电预警
    {
        halDigioIntSetEdge (&Pwr_Off_Alert, HAL_DIGIO_INT_RISING_EDGE); // 上升沿触发
        halDigioIntConnect (&Pwr_Off_Alert, POA_INT_RSP);               // 初始化中断，建立连接
        halDigioIntClear (&Pwr_Off_Alert);                              // 清中断标志
        halDigioIntEnable (&Pwr_Off_Alert);                             // 开启中断
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
    CSCTL0_H = CSKEY_H;                                     // 解锁CS模块

    Set_Freq (HAL_CPU_MCLK_4M);                             // 主频4M

    CSCTL2 = SELA__VLOCLK + SELS__DCOCLK + SELM__DCOCLK;    // ACLK =VLO    MCLK =SMCLK =DCO
    CSCTL3 = DIVA__1 + DIVS__1 + DIVM__1;                   // ACLK/1   SMCLK/1     MCLK/1

    CSCTL4 = HFXTOFF + LFXTDRIVE_1;                         // XT2关闭。   XT1开启，无源晶振，驱动1级，VLO on, SMCLK on

    init_diag ();                                           // 诊断初始化
    diag_clrscr ();                                         // 清屏
    diagnose ("MCLK: %uKHz.\n", HAL_CPU_CLOCK_KHz);         // 显示主频, KHz

    if (MAC_Init ())    diagnose ("MAC Successful.\n");
    else    diagnose ("MAC Failure.\n");

    init_button();                                         // 开启按键中断

    //  Energy      需要LF
    Energy_Volt_init ();                                    // 能量-电压 初始化
    Energy_Current_init ();                                 // 能量-电流 初始化

    while (DMA_IN_USE (DMA_CHANNEL_2));                     // 等待DIAG通信完毕
    while (DMA_IN_USE (DMA_CHANNEL_0));                     // 等待MAC通信完毕        共约0.5ms

    // -----------------    已执行12ms，仍然剩余约188ms      --------------------
    TA3CCR0 = 1500;                                         // 160 ~ 200ms
    TA3CTL = TASSEL_1 + ID_0 + MC_1 + TACLR +  TAIE;        // VLO/1, up mode, interrupt enabled
    __low_power_mode_3();                                   // LPM3
    TA3CTL = 0;

    // -----------------    剩余约8ms，检测晶振起振状态 ------------------------
    do
    {
        CSCTL5 &= ~LFXTOFFG;                // Clear XT1 fault flag
        SFRIFG1 &= ~OFIFG;
    } while (SFRIFG1 & OFIFG);              // Test oscillator fault flag

    CSCTL2 = SELA__LFXTCLK + SELS__DCOCLK + SELM__DCOCLK;   // ACLK =LFXT   MCLK =SMCLK =DCO
    CSCTL4 = HFXTOFF + LFXTDRIVE_0 + VLOOFF;                // XT2关闭。   XT1开启，无源晶振，驱动0级，VLO off, SMCLK on
    CSCTL0_H = 0;                                           // 上锁
    //  MCU_IO_PERIPHERAL_1 (J, 2, MCU_IO_DIR_OUTPUT);          // ACLK
    //  MCU_IO_PERIPHERAL_3 (7, 4, MCU_IO_DIR_OUTPUT);          // MCLK

    // *************************    低频晶振起振  ****************************************

    //  升压泵     需要LF
    Charge_pump_EN(CHARGE_PUMP_CLK_512);                    // 电压泵 512Hz
    //  LCD_C       需要LF        一旦开启LCD，不要关闭！！！
    LCD_on ();                                              // 段式液晶显示开。一旦开启LCD，不要关闭！！！


    //  RTC.    RX6110需要40ms以上启动时间，此初始化程序放在后面。      需要DCO + LF
    SystemClockinit();                                      // 设置系统时钟
    sync_flag = FALSE;

    digioConfig pinRTC_IRQ = {HAL_BOARD_RTC_INT_PORT, HAL_BOARD_RTC_INT_PIN, HAL_BOARD_RTC_INT_PIN_BIT, HAL_DIGIO_INPUT,  0};       // RTC INT
    uint16 rtc_status;

    rtc_status = rtc_valid_check ();
    if (rtc_status == HAL_BOARD_RX6110_READY)               // RTC正常，不再初始化，直接读取时间，同步
    {
//      rtc_read ();                                        // 读取实时时钟
        rtc_sync_en ();                                     // 在IRQ2上输出同步时钟。一定在开中断前开同步！！！

        halDigioIntSetEdge (&pinRTC_IRQ, HAL_DIGIO_INT_FALLING_EDGE);   // pinRTC_IRQ下降沿触发
        halDigioIntConnect (&pinRTC_IRQ, SYNC_isr);                     // 初始化中断，建立连接
        halDigioIntClear (&pinRTC_IRQ);                                 // 清中断标志
        halDigioIntEnable (&pinRTC_IRQ);                                // 开启同步中断
    }
    else if (rtc_status == HAL_BOARD_RX6110_VLF_FAIL)       // RTC掉电，重新初始化。
    {
        // 等1秒
        // 需通过网络同步
        // 此时应通过网络同步时间，同步后置sync_flag = FALSE;执行rtc_preset ();并在下一个同步点执行rtc_sync ();

        rtc_preset ();                                      // 设置实时时钟。注意若本次上电未能同步，则下次上电的同步信息是错误的。
        // rtc_sync ();                                     // 同步RTC时钟。执行本指令后，IRQ2立即回到低电位
    }
    else                                                    // RTC损坏或不存在
    {
        // 暂未考虑
    }

    //  rtc_power_off ();
}


void Set_Freq (uint8 level)                         // 设定主频。单位：0-6级 暂时只支持到8M
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


    CSCTL1_L = level <<1;                       // 设置频率，低速，最高到8M

}

void Charge_pump_EN (uint16 value)                      // 电荷泵开启。取值0-7，越大频率越高。0为关闭
{
    MCU_IO_PERIPHERAL_1 (HAL_CRG_PUMP_CLK_PORT, HAL_CRG_PUMP_CLK_PIN, MCU_IO_DIR_OUTPUT);   // TA0.1，    PWM: SEL_1

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
    duty = value /2;                                        // 50%占空比
    if (value != 0)     value--;

    TIMER_A_generatePWM (HAL_CRG_PUMP_PWM_BASE,
                         TIMER_A_CLOCKSOURCE_ACLK,          // ACLK, 32K
                         TIMER_A_CLOCKSOURCE_DIVIDER_1,     // ACLK/1
                         value,                             // 周期
                         HAL_CRG_PUMP_PWM_CNL,
                         TIMER_A_OUTPUTMODE_SET_RESET,      // 低电平脉冲
                         duty                               // 50%
                         );

    TIMER_A_startCounter (HAL_CRG_PUMP_PWM_BASE,
                          TIMER_A_UP_MODE                   // UP 模式开启
                          );
}

void POA_INT_RSP (void)                             // 掉电预警 中断响应
{
//  如果电压反复震荡，掉电信号可能不止一个。
//  在启动时如果电压下降过大也会产生该掉电信号。

    beep(14);                                           // 5ms
}

void Charge_pump_DIS (void)                         // 电荷泵停止
{
    Timer_A_stop (HAL_CRG_PUMP_PWM_BASE);                   // Timer STOP
}

void SYNC_isr(void)
{
    SystemClockSync();                                      // 秒同步
    sync_flag = TRUE;

    digioConfig pinRTC_IRQ = {HAL_BOARD_RTC_INT_PORT, HAL_BOARD_RTC_INT_PIN, HAL_BOARD_RTC_INT_PIN_BIT, HAL_DIGIO_INPUT,  0};       // RTC INT
    halDigioIntDisable (&pinRTC_IRQ);                       // 关闭同步中断
    rtc_sync_dis ();                                        // 在IRQ2上关闭同步时钟
}

uint32 sys_counter, sys_timer;                              // 32位计数器，1秒计时器
bool second_flag;

HAL_ISR_FUNCTION(Timer_A3,TIMER3_A1)
{
    uint16 i;
    TA3CTL &= ~TAIFG;

    sys_counter++;

#ifdef SYS_SLEEP_PERIOD                                     // 如睡眠时间大于等于1秒
    for (i=0; i<SYS_SLEEP_PERIOD; i++)
    {   sys_timer++;
        rtc_run ();     }                                   // 计时系统时钟，计入多秒
    __low_power_mode_off_on_exit();                         // Clear CPUOFF bit from 0(SR)
#else
    if ((counter%SYS_SLEEP_FREQ) == 0)
    {
        sys_timer++;
        rtc_run ();
        __low_power_mode_off_on_exit();                     // Clear CPUOFF bit from 0(SR)
    }                                                       // 多次计入1秒
#endif

    second_flag = TRUE;
}
