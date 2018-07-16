
#ifndef ENERGY_AWARE
#define ENERGY_AWARE

//  -----------------***************    ������֪����  **************----------------------

/*  �˲��ֹ�10��I/O��
    1.  C1����ʹ�ܣ�O��
    2.  C1��ѹ���루AD��
    3.  C2����ʹ�ܣ�O��
    4.  C2��ѹ���루AD��

    5.  �����������أ�O��
    6.  ����������ѹ���أ�PWM��
    7.  ��������1���루AD��
    8.  ��������1���㣨O��
    9.  ��������2���루AD��
    10. ��������2���㣨O��
*/


//  ----------------------------    ��������    --------------------------------
typedef struct
{
    uint32  Pri_cap_volt;                           // �����ݵ�ѹ��uV��
    uint32  Sec_cap_volt;                           // ���õ��ݵ�ѹ��uV��
    uint32  Harv_1st_charge;                        // ��ȡ����� ����1��uA��
    uint32  Harv_2nd_charge;                        // ��ȡ����� ����2��uA��
} Energy_info;
extern Energy_info Energy_measure_result;           // �����������




//  --------------------------------    ��ѹ����    ----------------------------


#define HAL_ADC_PS_VOLT                     2
#define HAL_ADC_PS_VOLT_PORT                1
#define HAL_ADC_PS_VOLT_PIN                 2

#define HAL_ADC_SCM_VOLT                    3
#define HAL_ADC_SCM_VOLT_PORT               1
#define HAL_ADC_SCM_VOLT_PIN                3



#define HAL_PS_VOLT_EN_PORT                 2
#define HAL_PS_VOLT_EN_PIN                  6       // ��������C1��ѹ

#define HAL_SCM_VOLT_EN_PORT                2
#define HAL_SCM_VOLT_EN_PIN                 5       // ��������C2��ѹ



#define     Meas_C1_en()    st( MCU_IO_OUTPUT (HAL_PS_VOLT_EN_PORT, HAL_PS_VOLT_EN_PIN, 1);         )
#define     Meas_C1_dis()   st( MCU_IO_OUTPUT (HAL_PS_VOLT_EN_PORT, HAL_PS_VOLT_EN_PIN, 0);         )

#define     Meas_C2_en()    st( MCU_IO_OUTPUT (HAL_SCM_VOLT_EN_PORT, HAL_SCM_VOLT_EN_PIN, 1);           )
#define     Meas_C2_dis()   st( MCU_IO_OUTPUT (HAL_SCM_VOLT_EN_PORT, HAL_SCM_VOLT_EN_PIN, 0);           )





//  --------------------------------    ��������    ----------------------------


#define HAL_ADC_PS_Harv_1st                 8
#define HAL_ADC_PS_Harv_1st_PORT            9
#define HAL_ADC_PS_Harv_1st_PIN             0

#define HAL_ADC_PS_Harv_2nd                 9
#define HAL_ADC_PS_Harv_2nd_PORT            9
#define HAL_ADC_PS_Harv_2nd_PIN             1



#define HAL_PS_ITG_EN_PORT                  3
#define HAL_PS_ITG_EN_PIN                   0       // �ܺĲ���ʹ��

#define HAL_PS_RESET_1st_PORT               6
#define HAL_PS_RESET_1st_PIN                0       // ��ɼ������㣨����1��

#define HAL_PS_RESET_2nd_PORT               2
#define HAL_PS_RESET_2nd_PIN                7       // ��ɼ������㣨����2��



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



#define     Amp_1st_clr_en()        st( MCU_IO_OUTPUT (HAL_PS_RESET_1st_PORT, HAL_PS_RESET_1st_PIN, 1);             )
#define     Amp_1st_clr_dis()       st( MCU_IO_OUTPUT (HAL_PS_RESET_1st_PORT, HAL_PS_RESET_1st_PIN, 0);             )

#define     Amp_2nd_clr_en()        st( MCU_IO_OUTPUT (HAL_PS_RESET_2nd_PORT, HAL_PS_RESET_2nd_PIN, 1);             )
#define     Amp_2nd_clr_dis()       st( MCU_IO_OUTPUT (HAL_PS_RESET_2nd_PORT, HAL_PS_RESET_2nd_PIN, 0);             )


#define     Meas_current_en()       st( MCU_IO_OUTPUT (HAL_PS_ITG_EN_PORT, HAL_PS_ITG_EN_PIN, 1);           )
#define     Meas_current_dis()      st( MCU_IO_OUTPUT (HAL_PS_ITG_EN_PORT, HAL_PS_ITG_EN_PIN, 0);           )





//  ----------------------------    ��������    --------------------------------


//  Ϊ��߳���ִ��Ч�ʣ�����7�����岻�ɸı�����ֵ������
#define     Energy_Voltage_C1                       0x01        // ����C1��ѹ
#define     Energy_Voltage_C2                       0x02        // ����C2��ѹ
#define     Energy_Sample_1st                       0x04        // ��������� ����1
#define     Energy_Sample_2nd                       0x08        // ��������� ����2
#define     Energy_Sample_and_Clear_1st             0x10        // ��������λ����� ����1
#define     Energy_Sample_and_Clear_2nd             0x20        // ��������λ����� ����2





extern void Energy_Volt_init (void);                    // ����-��ѹ ��ʼ��
extern void Energy_Current_init (void);             // ����-���� ��ʼ��
extern void Energy_Current_Disable (void);              // �رյ�������

extern uint8 Energy_Volt_Sense (uint8 _sense_mode);     // ��ѹ��֪
extern uint8 Energy_Current_Sense (uint8 _sense_mode);  // ������֪


extern void Inhibit_1st_Current_Channel (void);     // ���Ƶ�һ����ͨ��
extern void Inhibit_2nd_Current_Channel (void);     // ���Ƶڶ�����ͨ��
extern void Release_1st_Current_Channel (void);     // �ͷŵ�һ����ͨ��
extern void Release_2nd_Current_Channel (void);     // �ͷŵڶ�����ͨ��


void Current_Amp_EN (uint16 value);                     // ������������
void Current_Amp_DIS (void);                            // ��������ֹͣ







//  -----------------***************    �������Ʋ���  **************----------------------

/*  �˲��ֹ�3��I/O��
    1.  C2�����ƣ�PWM��
    2.  C2�ŵ�ʹ�ܣ�O��
    3.  C2�ŵ���ѹ�ã�PWM��
*/


#define HAL_SCM_CRG_PORT                    7
#define HAL_SCM_CRG_PIN                     7       // C2���, TA1.2
#define HAL_SCM_CRG_PWM_BASE                TIMER_A1_BASE
#define HAL_SCM_CRG_PWM_CNL                 TIMER_A_CAPTURECOMPARE_REGISTER_2
#define HAL_SCM_CRG_PEROID                  TA1CCR0


#define HAL_SCM_NEG_PUMP_PORT               3
#define HAL_SCM_NEG_PUMP_PIN                3       // C2�ŵ縺ѹ��, TA1.1
#define HAL_SCM_NEG_PUMP_PWM_BASE           TIMER_A1_BASE
#define HAL_SCM_NEG_PUMP_PWM_CNL            TIMER_A_CAPTURECOMPARE_REGISTER_1
#define HAL_SCM_NEG_PUMP_PEROID             TA1CCR0


#define HAL_SCM_DISCRG_PORT                 6
#define HAL_SCM_DISCRG_PIN                  7       // C2�ŵ�



//  ----------------------------    ��������    --------------------------------

#define     Energy_Discharge_EN()       st( MCU_IO_OUTPUT (HAL_SCM_DISCRG_PORT, HAL_SCM_DISCRG_PIN, 1);             )
#define     Energy_Discharge_DIS()      st( MCU_IO_OUTPUT (HAL_SCM_DISCRG_PORT, HAL_SCM_DISCRG_PIN, 0);             )



extern void C2_Charge_EN (uint16 value);                // �򳬼����ݳ�硣ȡֵ0-1000��ԽСԽ�죬1000Ϊֹͣ��硣����ٶȼ���������1.4.1�������ԣ�
extern void C2_Charge_DIS (void);                       // ֹͣ�򳬼����ݳ��
extern void C2_Discharge_Pump_EN (void);                // C2�ŵ�ÿ�����1KHz������3���Ӽ���
extern void C2_Discharge_Pump_DIS (void);               // C2�ŵ�ùر�




#endif
