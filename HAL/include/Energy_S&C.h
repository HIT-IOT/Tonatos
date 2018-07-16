
#ifndef ENERGY_AWARE
#define ENERGY_AWARE

//  -----------------***************    能量感知部分  **************----------------------

/*  此部分共10个I/O：
    1.  C1测量使能（O）
    2.  C1电压输入（AD）
    3.  C2测量使能（O）
    4.  C2电压输入（AD）

    5.  电流测量开关（O）
    6.  电流测量升压开关（PWM）
    7.  电流量程1输入（AD）
    8.  电流量程1清零（O）
    9.  电流量程2输入（AD）
    10. 电流量程2清零（O）
*/


//  ----------------------------    变量声明    --------------------------------
typedef struct
{
    uint32  Pri_cap_volt;                           // 主电容电压（uV）
    uint32  Sec_cap_volt;                           // 备用电容电压（uV）
    uint32  Harv_1st_charge;                        // 获取电荷量 量程1（uA）
    uint32  Harv_2nd_charge;                        // 获取电荷量 量程2（uA）
} Energy_info;
extern Energy_info Energy_measure_result;           // 能量测量结果




//  --------------------------------    电压测量    ----------------------------


#define HAL_ADC_PS_VOLT                     2
#define HAL_ADC_PS_VOLT_PORT                1
#define HAL_ADC_PS_VOLT_PIN                 2

#define HAL_ADC_SCM_VOLT                    3
#define HAL_ADC_SCM_VOLT_PORT               1
#define HAL_ADC_SCM_VOLT_PIN                3



#define HAL_PS_VOLT_EN_PORT                 2
#define HAL_PS_VOLT_EN_PIN                  6       // 测量电容C1电压

#define HAL_SCM_VOLT_EN_PORT                2
#define HAL_SCM_VOLT_EN_PIN                 5       // 测量电容C2电压



#define     Meas_C1_en()    st( MCU_IO_OUTPUT (HAL_PS_VOLT_EN_PORT, HAL_PS_VOLT_EN_PIN, 1);         )
#define     Meas_C1_dis()   st( MCU_IO_OUTPUT (HAL_PS_VOLT_EN_PORT, HAL_PS_VOLT_EN_PIN, 0);         )

#define     Meas_C2_en()    st( MCU_IO_OUTPUT (HAL_SCM_VOLT_EN_PORT, HAL_SCM_VOLT_EN_PIN, 1);           )
#define     Meas_C2_dis()   st( MCU_IO_OUTPUT (HAL_SCM_VOLT_EN_PORT, HAL_SCM_VOLT_EN_PIN, 0);           )





//  --------------------------------    电流测量    ----------------------------


#define HAL_ADC_PS_Harv_1st                 8
#define HAL_ADC_PS_Harv_1st_PORT            9
#define HAL_ADC_PS_Harv_1st_PIN             0

#define HAL_ADC_PS_Harv_2nd                 9
#define HAL_ADC_PS_Harv_2nd_PORT            9
#define HAL_ADC_PS_Harv_2nd_PIN             1



#define HAL_PS_ITG_EN_PORT                  3
#define HAL_PS_ITG_EN_PIN                   0       // 能耗测量使能

#define HAL_PS_RESET_1st_PORT               6
#define HAL_PS_RESET_1st_PIN                0       // 电荷计量清零（量程1）

#define HAL_PS_RESET_2nd_PORT               2
#define HAL_PS_RESET_2nd_PIN                7       // 电荷计量清零（量程2）



#define HAL_CRG_PUMP_CLK_PORT               7
#define HAL_CRG_PUMP_CLK_PIN                6       // 电荷泵时钟, TA0.1
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





//  ----------------------------    函数声明    --------------------------------


//  为提高程序执行效率，以下7条定义不可改变其数值！！！
#define     Energy_Voltage_C1                       0x01        // 测量C1电压
#define     Energy_Voltage_C2                       0x02        // 测量C2电压
#define     Energy_Sample_1st                       0x04        // 采样电荷量 量程1
#define     Energy_Sample_2nd                       0x08        // 采样电荷量 量程2
#define     Energy_Sample_and_Clear_1st             0x10        // 采样并复位电荷量 量程1
#define     Energy_Sample_and_Clear_2nd             0x20        // 采样并复位电荷量 量程2





extern void Energy_Volt_init (void);                    // 能量-电压 初始化
extern void Energy_Current_init (void);             // 能量-电流 初始化
extern void Energy_Current_Disable (void);              // 关闭电流测量

extern uint8 Energy_Volt_Sense (uint8 _sense_mode);     // 电压感知
extern uint8 Energy_Current_Sense (uint8 _sense_mode);  // 电流感知


extern void Inhibit_1st_Current_Channel (void);     // 抑制第一电流通道
extern void Inhibit_2nd_Current_Channel (void);     // 抑制第二电流通道
extern void Release_1st_Current_Channel (void);     // 释放第一电流通道
extern void Release_2nd_Current_Channel (void);     // 释放第二电流通道


void Current_Amp_EN (uint16 value);                     // 电流测量开启
void Current_Amp_DIS (void);                            // 电流测量停止







//  -----------------***************    能量控制部分  **************----------------------

/*  此部分共3个I/O：
    1.  C2充电控制（PWM）
    2.  C2放电使能（O）
    3.  C2放电升压泵（PWM）
*/


#define HAL_SCM_CRG_PORT                    7
#define HAL_SCM_CRG_PIN                     7       // C2充电, TA1.2
#define HAL_SCM_CRG_PWM_BASE                TIMER_A1_BASE
#define HAL_SCM_CRG_PWM_CNL                 TIMER_A_CAPTURECOMPARE_REGISTER_2
#define HAL_SCM_CRG_PEROID                  TA1CCR0


#define HAL_SCM_NEG_PUMP_PORT               3
#define HAL_SCM_NEG_PUMP_PIN                3       // C2放电负压泵, TA1.1
#define HAL_SCM_NEG_PUMP_PWM_BASE           TIMER_A1_BASE
#define HAL_SCM_NEG_PUMP_PWM_CNL            TIMER_A_CAPTURECOMPARE_REGISTER_1
#define HAL_SCM_NEG_PUMP_PEROID             TA1CCR0


#define HAL_SCM_DISCRG_PORT                 6
#define HAL_SCM_DISCRG_PIN                  7       // C2放电



//  ----------------------------    函数声明    --------------------------------

#define     Energy_Discharge_EN()       st( MCU_IO_OUTPUT (HAL_SCM_DISCRG_PORT, HAL_SCM_DISCRG_PIN, 1);             )
#define     Energy_Discharge_DIS()      st( MCU_IO_OUTPUT (HAL_SCM_DISCRG_PORT, HAL_SCM_DISCRG_PIN, 0);             )



extern void C2_Charge_EN (uint16 value);                // 向超级电容充电。取值0-1000，越小越快，1000为停止充电。充电速度见调试数据1.4.1（非线性）
extern void C2_Charge_DIS (void);                       // 停止向超级电容充电
extern void C2_Discharge_Pump_EN (void);                // C2放电泵开启，1KHz，开启3秒钟即可
extern void C2_Discharge_Pump_DIS (void);               // C2放电泵关闭




#endif
