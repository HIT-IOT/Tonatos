/*
 * hal.h
 *
 *  Created on: 2018��4��26��
 *      Author: carpela
 */

#ifndef HAL_INCLUDE_HAL_H_
#define HAL_INCLUDE_HAL_H_

#include <types.h>

typedef struct                              // ϵͳ״̬
{
    union
    {
        uint8 status;
        struct
        {
            unsigned char RUNNING           : 1;    // ������
            unsigned char IDLE              : 1;    // ����
            unsigned char SLEEP             : 1;    // ����
            unsigned char                   : 1;    //
            unsigned char                   : 1;    //
            unsigned char                   : 1;    //
            unsigned char INITIALIZING      : 1;    // ��ʼ�����״�������
            unsigned char ERROR             : 1;    // ����
        } bits;
    } mode;

    union
    {
        uint8 status;
        struct
        {
            unsigned char LOW_PWR               : 1;    // ����
            unsigned char CHARGING          : 1;    // �����
            unsigned char CHARGED               : 1;    // ����
            unsigned char OVER_VOLT         : 1;    // ����ѹ
            unsigned char                   : 1;    //
            unsigned char SAVING                : 1;    // ������
            unsigned char AUX_PWR               : 1;    // ����Դ������
            unsigned char MAIN_PWR          : 1;    // ����Դ������
        } bits;
    } power;

uint16 vcc_voltage;                             // ϵͳ��ѹ
uint16 Pri_voltage;                             // ��ص�ѹ
uint16 Aux_voltage;                             // ������ص�ѹ
} SysStatus;

// -------------------------- ʱ�� ------------------------------------------
#define HAL_CPU_MCLK_1M                 0
#define HAL_CPU_MCLK_2M                 1
#define HAL_CPU_MCLK_3M                 2
#define HAL_CPU_MCLK_4M                 3
#define HAL_CPU_MCLK_5M                 4
#define HAL_CPU_MCLK_7M                 5
#define HAL_CPU_MCLK_8M                 6

//  #define HAL_CPU_CLOCK_FIX               TRUE
//  #define HAL_CPU_CLOCK_MHZ               7.3728


#define HAL_UART_CLOCK_MHZ              4.194304

//  ----------------------------    ����Ԥ��    --------------------------------
#define HAL_PWR_ALERT_PORT                  1
#define HAL_PWR_ALERT_PIN                   1
#define HAL_PWR_ALERT_PIN_BIT               BIT1



//  --------------------------------    AD����    --------------------------------

#define HAL_ADC_EXT1                        8
#define HAL_ADC_EXT1_PORT                   9
#define HAL_ADC_EXT1_PIN                    0

#define HAL_ADC_EXT2                        9
#define HAL_ADC_EXT2_PORT                   9
#define HAL_ADC_EXT2_PIN                    1

#define HAL_ADC_SS_ACC_X                    12
#define HAL_ADC_SS_ACC_X_PORT               9
#define HAL_ADC_SS_ACC_X_PIN                4

#define HAL_ADC_SS_ACC_Y                    13
#define HAL_ADC_SS_ACC_Y_PORT               9
#define HAL_ADC_SS_ACC_Y_PIN                5

#define HAL_ADC_SS_ACC_Z                    14
#define HAL_ADC_SS_ACC_Z_PORT               9
#define HAL_ADC_SS_ACC_Z_PIN                6

#define HAL_ADC_SS_LUX                      15
#define HAL_ADC_SS_LUX_PORT                 9
#define HAL_ADC_SS_LUX_PIN                  7



//  --------------------------------    ����  --------------------------------


#define HAL_CRG_PUMP_CLK_PORT               7
#define HAL_CRG_PUMP_CLK_PIN                6       // ��ɱ�ʱ��, TA0.1
#define HAL_CRG_PUMP_PWM_BASE               TIMER_A0_BASE
#define HAL_CRG_PUMP_PWM_CNL                TIMER_A_CAPTURECOMPARE_REGISTER_1
#define HAL_CRG_PUMP_PEROID                 TA0CCR0


#define CHARGE_PUMP_CLK_0                   0
#define CHARGE_PUMP_CLK_256                 1
#define CHARGE_PUMP_CLK_512                 2
#define CHARGE_PUMP_CLK_1K                  3
#define CHARGE_PUMP_CLK_2K                  4
#define CHARGE_PUMP_CLK_4K                  5
#define CHARGE_PUMP_CLK_8K                  6
#define CHARGE_PUMP_CLK_16K                 7


#define HAL_SS_POWER_PORT                   2
#define HAL_SS_POWER_PIN                    4       // ��������Դ




extern bool sync_flag;                  // ͬ����־
extern SysStatus sys_status;                  // ϵͳ״̬
extern uint16 HAL_CPU_CLOCK_KHz;            // ��Ƶ. ��λ��KHz. up to 65M
extern uint16 HAL_CPU_CLOCK_DELAY;          // ��ʱ����ר�ã��������

/*******************************************************************************
    ��������
*/



#define     Sensors_on()        st( MCU_IO_OUTPUT (HAL_SS_POWER_PORT, HAL_SS_POWER_PIN, 1);             )
#define     Sensors_off()       st( MCU_IO_OUTPUT (HAL_SS_POWER_PORT, HAL_SS_POWER_PIN, 0);             )

extern void hal_init(void);
extern void Set_Freq (uint8 level);     // �趨��Ƶ����λ��Hz   ��ʱֻ֧�ֵ�8M
extern void Charge_pump_EN (uint16 value);// ��ɱÿ���
extern void Charge_pump_DIS (void);     // ��ɱ�ֹͣ
extern bool second_flag;
#endif /* HAL_INCLUDE_HAL_H_ */
