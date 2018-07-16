#include <ADC_B_Convert.h>
#include <Energy_S&C.h>
#include <bsp_timer_a.h>
#include <RTC.h>
#include <hal_mcu.h>
#include <hal.h>
#include <macros.h>
#include <msp430.h>
#include <bsp_timer_a.h>

#define     _Vcc_Voltage            2120                // VCC电压（单位：mV）

Energy_info     Energy_measure_result;                  // 能量测量结果

static bool Energy_volt_enable = false;             // 电压测量使能
static bool Energy_current_enable = false;          // 电流测量使能
static bool Energy_current_1st_Inhibitor = false;   // 电流第一通道抑制器
static bool Energy_current_2nd_Inhibitor = false;   // 电流第二通道抑制器


static uint16   _init_PS_Harv_1st, _init_PS_Harv_2nd;   // 内部变量，电流计量起始点





void Energy_Volt_init (void)                                            // 能量-电压 初始化
{
    MCU_IO_PERIPHERAL_3 (HAL_ADC_PS_VOLT_PORT, HAL_ADC_PS_VOLT_PIN, MCU_IO_DIR_INPUT);   // 主电容电压检测
    MCU_IO_PERIPHERAL_3 (HAL_ADC_SCM_VOLT_PORT, HAL_ADC_SCM_VOLT_PIN, MCU_IO_DIR_INPUT); // 副电容电压检测


//  ------------------------    初始化AD转换通道   --------------------------------

    //Configure Memory Buffer
    /*
        MEM0、MEM1为电压检测。参考源均为AVCC和AGND，不考虑窗口和差分
        MEM0： HAL_ADC_PS_VOLT
        MEM1： HAL_ADC_SCM_VOLT              完成后引发中断
    */

       /*
         * Base address of the ADC12B Module
         * Configure memory buffer 0
         * Map HAL_ADC_PS_VOLT to memory buffer 0
         * Vref+ = AVcc
         * Vref- = AVss
         * Memory buffer 0 is not the end of a sequence
         */
        ADC12_B_memoryConfigure(ADC12_B_BASE,
                                ADC12_B_MEMORY_0,
                                HAL_ADC_PS_VOLT,
                                ADC12_B_VREFPOS_AVCC_VREFNEG_VSS,
                                ADC12_B_NOTENDOFSEQUENCE,
                                ADC12_B_WINDOW_COMPARATOR_DISABLE,
                                ADC12_B_DIFFERENTIAL_MODE_DISABLE
                                );
        /*
         * Base address of the ADC12B Module
         * Configure memory buffer 1
         * Map HAL_ADC_SCM_VOLT to memory buffer 1
         * Vref+ = AVcc
         * Vref- = AVss
         * Memory buffer 1 is not the end of a sequence
         */
        ADC12_B_memoryConfigure(ADC12_B_BASE,
                                ADC12_B_MEMORY_1,
                                HAL_ADC_SCM_VOLT,
                                ADC12_B_VREFPOS_AVCC_VREFNEG_VSS,
                                ADC12_B_ENDOFSEQUENCE,                          // end of a sequence
                                ADC12_B_WINDOW_COMPARATOR_DISABLE,
                                ADC12_B_DIFFERENTIAL_MODE_DISABLE
                                );


//  ------------------------    通道初始化完毕 --------------------------------

        ADC12_B_clearInterrupt(ADC12_B_BASE,
                               0,                                               // 0段，MEM0~MEM15
                               ADC12_B_IFG1                                     // 清空 MEM1 标志
                               );

        //Enable memory buffer 1 interrupt
        ADC12_B_enableInterrupt(ADC12_B_BASE,
                                ADC12_B_IE1,                                    // MEM0~MEM15       使能 MEM1 中断
                                0,                                              // MEM16~MEM31
                                0);                                             // 其它

//  ----------------------------    初始化变量   --------------------------------

    Energy_measure_result.Pri_cap_volt = 0;
    Energy_measure_result.Sec_cap_volt = 0;

    Energy_volt_enable = true;                                                  // 电压测量使能
}



void Energy_Current_init (void)                                     // 能量-电流 初始化
{
    MCU_IO_PERIPHERAL_3 (HAL_ADC_PS_Harv_1st_PORT, HAL_ADC_PS_Harv_1st_PIN, MCU_IO_DIR_INPUT);  // 收获电荷检测（量程1）
    MCU_IO_PERIPHERAL_3 (HAL_ADC_PS_Harv_2nd_PORT, HAL_ADC_PS_Harv_2nd_PIN, MCU_IO_DIR_INPUT);  // 收获电荷检测（量程2）


//  ------------------------    初始化AD转换通道   --------------------------------

    //Configure Memory Buffer
    /*
        MEM2、MEM3为电流检测。参考源均为AVCC和AGND，不考虑窗口和差分
        MEM2： HAL_ADC_PS_Harv_1st
        MEM3： HAL_ADC_PS_Harv_2nd           完成后引发中断
    */

        /*
         * Base address of the ADC12B Module
         * Configure memory buffer 2
         * Map HAL_ADC_PS_Harv_1st to memory buffer 2
         * Vref+ = AVcc
         * Vref- = AVss
         * Memory buffer 2 is not the end of a sequence
         */
        ADC12_B_memoryConfigure(ADC12_B_BASE,
                                ADC12_B_MEMORY_2,
                                HAL_ADC_PS_Harv_1st,
                                ADC12_B_VREFPOS_AVCC_VREFNEG_VSS,
                                ADC12_B_NOTENDOFSEQUENCE,
                                ADC12_B_WINDOW_COMPARATOR_DISABLE,
                                ADC12_B_DIFFERENTIAL_MODE_DISABLE
                                );
        /*
         * Base address of the ADC12B Module
         * Configure memory buffer 3
         * Map HAL_ADC_PS_Harv_2nd to memory buffer 3
         * Vref+ = AVcc
         * Vref- = AVss
         * Memory buffer 3 is the end of a sequence
         */
        ADC12_B_memoryConfigure(ADC12_B_BASE,
                                ADC12_B_MEMORY_3,
                                HAL_ADC_PS_Harv_2nd,
                                ADC12_B_VREFPOS_AVCC_VREFNEG_VSS,
                                ADC12_B_ENDOFSEQUENCE,                          // end of a sequence
                                ADC12_B_WINDOW_COMPARATOR_DISABLE,
                                ADC12_B_DIFFERENTIAL_MODE_DISABLE
                                );


//  ------------------------    通道初始化完毕 --------------------------------

        ADC12_B_clearInterrupt(ADC12_B_BASE,
                               0,                                               // 0段，MEM0~MEM15
                               ADC12_B_IFG3                                     // 清空 MEM3 标志
                               );

        //Enable memory buffer 3 interrupt
        ADC12_B_enableInterrupt(ADC12_B_BASE,
                                ADC12_B_IE3,                                    // MEM0~MEM15       使能 MEM3 中断
                                0,                                              // MEM16~MEM31
                                0);                                             // 其它


//  ----------------------------    初始化硬件   --------------------------------

    Current_Amp_EN (CHARGE_PUMP_CLK_512);                                       // 电流测量开启，升压泵 512Hz


//  ----------------------------    初始化变量   --------------------------------

    _init_PS_Harv_1st = 0;
    _init_PS_Harv_2nd = 0;
    Energy_measure_result.Harv_1st_charge = 0;
    Energy_measure_result.Harv_2nd_charge = 0;

    Energy_current_1st_Inhibitor = false;                                       // 电流第一通道抑制器无效
    Energy_current_2nd_Inhibitor = false;                                       // 电流第二通道抑制器无效
    Amp_1st_clr_dis();
    Amp_2nd_clr_dis();                                                          // 放开电容复位

    Energy_current_enable = true;                                               // 电流测量使能
}


void Energy_Current_Disable (void)                                              // 关闭电流测量
{
    Energy_current_1st_Inhibitor = false;                                       // 电流第一通道抑制器无效
    Energy_current_2nd_Inhibitor = false;                                       // 电流第二通道抑制器无效

    Amp_1st_clr_dis();
    Amp_2nd_clr_dis();                                                          // 放开电容复位

    Current_Amp_DIS ();                                                         // 电流测量停止
    Energy_current_enable = false;                                              // 测量失效
}





uint8 Energy_Volt_Sense (uint8 _sense_mode)                             // 电压感知
{
    if (!Energy_volt_enable)    return(false);                                  // 未初始化，退出
    if (_sense_mode >= 0x04)    return(false);                                  // 参数错误，退出
    if (_sense_mode == 0)       return(false);                                  // 参数错误，退出

    uint32 _convertA, _convertB;

    if ((_sense_mode & Energy_Voltage_C1) != 0)     Meas_C1_en();               // 是否测量C1
    if ((_sense_mode & Energy_Voltage_C2) != 0)     Meas_C2_en();               // 是否测量C2
    halMcuWaitUs (25);                                                          // 等待建立电压

    ADC_convert_sequence (ADC12_B_START_AT_ADC12MEM0);                          // 采样电压
    Meas_C1_dis();
    Meas_C2_dis();


    if ((_sense_mode & Energy_Voltage_C1) != 0)                                 // 是否计算C1
    {
        _convertA = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_0);         // HAL_ADC_PS_VOLT      倍数 = 电阻(200K+100K)/100K = 3
        _convertA = (_convertA * _Vcc_Voltage * 375)>>9;                        // 单位：微伏            = _Vcc_Voltage*3*1000/4096
        Energy_measure_result.Pri_cap_volt = _convertA;
    }

    if ((_sense_mode & Energy_Voltage_C2) != 0)                                 // 是否计算C2
    {
        _convertB = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_1);         // HAL_ADC_SCM_VOLT     倍数 = 电阻(200K+100K)/100K = 3
        _convertB = (_convertB * _Vcc_Voltage * 375)>>9;                        // 单位：微伏            = _Vcc_Voltage*3*1000/4096
        Energy_measure_result.Sec_cap_volt = _convertB;
    }

    return (true);
}



uint8 Energy_Current_Sense (uint8 _sense_mode)                          // 电流感知
{
    if (!Energy_current_enable) return(false);                                  // 未初始化，退出
    if (_sense_mode >= 0x40)    return(false);                                  // 参数错误，退出
    if (_sense_mode < 0x04)     return(false);                                  // 参数错误，退出


    uint32 _convertA, _convertB;


    ADC_convert_sequence (ADC12_B_START_AT_ADC12MEM2);                          // 采样电流

    if ((_sense_mode & (Energy_Sample_1st + Energy_Sample_and_Clear_1st)) != 0) // 是否测量量程1
    {
        if ((_sense_mode & Energy_Sample_and_Clear_1st) != 0)                   // 是否计量清零。注：未开启测量模块时该操作会浪费能量
            Amp_1st_clr_en();                                                   // 计量电容清零29us
        //  ------------
        _convertA = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_2);         // HAL_ADC_PS_Harv_1st
        if (_convertA > _init_PS_Harv_1st)      _convertA -= _init_PS_Harv_1st; // 减初始值
        else _convertA = 0;
        _convertA = (_convertA * _Vcc_Voltage * 125) >>9;                       // 单位：微伏            = _Vcc_Voltage*1000/4096
        Energy_measure_result.Harv_1st_charge = _convertA;
        //  ------------    4M主频，计算时长29us
        if (!Energy_current_1st_Inhibitor)  Amp_1st_clr_dis();                  // 如该通道正常，停止计量电容清零
    }

    if ((_sense_mode & (Energy_Sample_2nd + Energy_Sample_and_Clear_2nd)) != 0) // 是否测量量程2
    {
        if ((_sense_mode & Energy_Sample_and_Clear_2nd) != 0)                   // 是否计量清零
            Amp_2nd_clr_en();                                                   // 计量电容清零29us
        //  ------------
        _convertB = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_3);         // HAL_ADC_PS_Harv_2nd
        if (_convertB > _init_PS_Harv_2nd)      _convertB -= _init_PS_Harv_2nd; // 减初始值
        else _convertB = 0;
        _convertB = (_convertB * _Vcc_Voltage * 125) >>9;                       // 单位：微伏            = _Vcc_Voltage*1000/4096
        Energy_measure_result.Harv_2nd_charge = _convertB;
        //  ------------    4M主频，计算时长29us
        if (!Energy_current_2nd_Inhibitor)  Amp_2nd_clr_dis();                  // 如该通道正常，停止计量电容清零
    }


    if (_sense_mode >= Energy_Sample_and_Clear_1st)
    {
        if ((((_sense_mode & Energy_Sample_and_Clear_1st) != 0) && (!Energy_current_1st_Inhibitor))||(((_sense_mode & Energy_Sample_and_Clear_2nd) != 0) && (!Energy_current_2nd_Inhibitor)))   // 是否需要2ms延时
        {
            // 运放反应需要时间！！！ 此处应留出充足的时间，否则差值计算出错！！！       ****************************************
            // 休眠等待2毫秒，临时使用TA3.1
            uint16 _delaytime;
            _delaytime = GetSystemClock() + (Standard_1ms *2);                      // 稍微不到2ms
            if (_delaytime >= RTC_UP_BOUND)     _delaytime -= RTC_UP_BOUND;

            TIMER_A_clearCaptureCompareInterruptFlag (TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);    // 清中断
            TIMER_A_initCompare (TIMER_A3_BASE,
                                 TIMER_A_CAPTURECOMPARE_REGISTER_1,
                                 TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,
                                 TIMER_A_OUTPUTMODE_OUTBITVALUE,
                                 _delaytime     );                                  // 开启TA3.1比较器及中断
            do
            {   __low_power_mode_3();                                               // LPM3
            }while (!TIMER_A_getCaptureCompareInterruptStatus (TIMER_A3_BASE,
                                                                TIMER_A_CAPTURECOMPARE_REGISTER_1,
                                                                TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG)
                    );                                                              // 如已到时间，跳出
        }   // -----------------------------------------------------------------------------------------------------------------

        ADC_convert_sequence (ADC12_B_START_AT_ADC12MEM2);                      // 采样初始电量

        if ((_sense_mode & Energy_Sample_and_Clear_1st) != 0)   _init_PS_Harv_1st = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_2);     // 记录清零后初始值，差值计算
        if ((_sense_mode & Energy_Sample_and_Clear_2nd) != 0)   _init_PS_Harv_2nd = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_3);     // 记录清零后初始值，差值计算
    }

    return (true);
}



void Inhibit_1st_Current_Channel (void)                                     // 抑制第一电流通道
{
    if (!Energy_current_enable) return;                                         // 未初始化，退出

    Energy_current_1st_Inhibitor = true;
    Amp_1st_clr_en();
}


void Inhibit_2nd_Current_Channel (void)                                     // 抑制第二电流通道
{
    if (!Energy_current_enable) return;                                         // 未初始化，退出

    Energy_current_2nd_Inhibitor = true;
    Amp_2nd_clr_en();
}


void Release_1st_Current_Channel (void)                                     // 释放第一电流通道
{
    if (!Energy_current_enable) return;                                         // 未初始化，退出

    Energy_current_1st_Inhibitor = false;
    Amp_1st_clr_dis();
}


void Release_2nd_Current_Channel (void)                                     // 释放第二电流通道
{
    if (!Energy_current_enable) return;                                         // 未初始化，退出

    Energy_current_2nd_Inhibitor = false;
    Amp_2nd_clr_dis();
}





void Current_Amp_EN (uint16 value)                                              // 电流测量开启。取值0-7，越大频率越高。0为关闭
{
    Meas_current_en();                                                          // 开启运放供电

    MCU_IO_PERIPHERAL_1 (HAL_CRG_PUMP_CLK_PORT, HAL_CRG_PUMP_CLK_PIN, MCU_IO_DIR_OUTPUT);   // TA0.1，    PWM: SEL_1

    if (value > CHARGE_PUMP_CLK_16K)    value = CHARGE_PUMP_CLK_16K;

    switch (value)
    {
        case CHARGE_PUMP_CLK_16K:   value = 2;      break;              // 32K/2 = 16K
        case CHARGE_PUMP_CLK_8K:    value = 4;      break;              // 32K/4 = 8K
        case CHARGE_PUMP_CLK_4K:    value = 8;      break;              // 32K/8 = 4K
        case CHARGE_PUMP_CLK_2K:    value = 16;     break;              // 32K/16 = 2K
        case CHARGE_PUMP_CLK_1K:    value = 32;     break;              // 32K/32 = 1K
        case CHARGE_PUMP_CLK_512:   value = 64;     break;              // 32K/64 = 512Hz
        case CHARGE_PUMP_CLK_256:   value = 128;    break;              // 32K/128 = 256Hz
        case CHARGE_PUMP_CLK_0:     value = 0;      break;              // 0Hz
    }

    uint16 duty;
    duty = value /2;                                                    // 50%占空比
    if (value != 0)     value--;

    TIMER_A_generatePWM (HAL_CRG_PUMP_PWM_BASE,
                         TIMER_A_CLOCKSOURCE_ACLK,                      // ACLK, 32K
                         TIMER_A_CLOCKSOURCE_DIVIDER_1,                 // ACLK/1
                         value,                                         // 周期
                         HAL_CRG_PUMP_PWM_CNL,
                         TIMER_A_OUTPUTMODE_SET_RESET,                  // 低电平脉冲
                         duty                                           // 50%
                         );

    TIMER_A_startCounter (HAL_CRG_PUMP_PWM_BASE,
                          TIMER_A_UP_MODE                               // UP 模式开启
                          );
}


void Current_Amp_DIS (void)                                                 // 电流测量停止
{
    TIMER_A_generatePWM (HAL_CRG_PUMP_PWM_BASE,
                         TIMER_A_CLOCKSOURCE_ACLK,
                         TIMER_A_CLOCKSOURCE_DIVIDER_1,
                         0,
                         HAL_CRG_PUMP_PWM_CNL,
                         TIMER_A_OUTPUTMODE_OUTBITVALUE,                // 关闭输出
                         1
                         );

    TIMER_A_stop (HAL_CRG_PUMP_PWM_BASE);                               // Timer STOP

    Meas_current_dis ();                                                // 关闭运放供电
}





void C2_Charge_EN (uint16 value)                                                // 向超级电容充电。取值0-1000，越小越快，1000为停止充电。充电速度见调试数据1.4.1（非线性）
{
    MCU_IO_PERIPHERAL_2 (HAL_SCM_CRG_PORT, HAL_SCM_CRG_PIN, MCU_IO_DIR_OUTPUT); // TA1.2,   PWM: SEL_2

    C2_Discharge_Pump_DIS ();                                                   // 不可与C2_Discharge_Pump_EN同用！！！

    TIMER_A_generatePWM (HAL_SCM_CRG_PWM_BASE,
                         TIMER_A_CLOCKSOURCE_ACLK,                  // ACLK, 32K
                         TIMER_A_CLOCKSOURCE_DIVIDER_1,             // ACLK/1
                         32768,
                         HAL_SCM_CRG_PWM_CNL,
                         TIMER_A_OUTPUTMODE_SET_RESET,              // 低电平脉冲
                         1                                          // 时间 1
                         );

    if (value >= 1000)  HAL_SCM_CRG_PEROID = 0;
    else    if (value < 2)  value = 2;
    else    HAL_SCM_CRG_PEROID = value;                             // 脉宽为1时间单位，调整周期，PFM模式

    TIMER_A_startCounter (HAL_SCM_CRG_PWM_BASE,
                          TIMER_A_UP_MODE                           // UP 模式开启
                          );
}


void C2_Charge_DIS (void)                                                       // 停止向超级电容充电
{
    TIMER_A_generatePWM (HAL_SCM_CRG_PWM_BASE,
                         TIMER_A_CLOCKSOURCE_ACLK,
                         TIMER_A_CLOCKSOURCE_DIVIDER_8,
                         0,
                         HAL_SCM_CRG_PWM_CNL,
                         TIMER_A_OUTPUTMODE_OUTBITVALUE,            // 关闭输出
                         1
                         );

    TIMER_A_stop (HAL_SCM_CRG_PWM_BASE);                            // Timer STOP
}



void C2_Discharge_Pump_EN (void)                                                // C2放电泵开启，1KHz，开启3秒钟即可
{
    MCU_IO_PERIPHERAL_2 (HAL_SCM_NEG_PUMP_PORT, HAL_SCM_NEG_PUMP_PIN, MCU_IO_DIR_OUTPUT);   // TA1.1,   PWM: SEL_2

    C2_Charge_DIS ();                                                           // 不可与C2_Charge_EN同用！！！

    TIMER_A_generatePWM (HAL_SCM_NEG_PUMP_PWM_BASE,
                         TIMER_A_CLOCKSOURCE_ACLK,                  // ACLK, 32K
                         TIMER_A_CLOCKSOURCE_DIVIDER_1,             // ACLK/1
                         31,                                        // 频率1KHz
                         HAL_SCM_NEG_PUMP_PWM_CNL,
                         TIMER_A_OUTPUTMODE_SET_RESET,              // 低电平脉冲
                         16                                         // 50%
                         );

    TIMER_A_startCounter (HAL_SCM_NEG_PUMP_PWM_BASE,
                          TIMER_A_UP_MODE                           // UP 模式开启
                          );
}


void C2_Discharge_Pump_DIS (void)                                               // C2放电泵关闭
{
    TIMER_A_generatePWM (HAL_SCM_NEG_PUMP_PWM_BASE,
                         TIMER_A_CLOCKSOURCE_ACLK,
                         TIMER_A_CLOCKSOURCE_DIVIDER_1,
                         0,
                         HAL_SCM_NEG_PUMP_PWM_CNL,
                         TIMER_A_OUTPUTMODE_OUTBITVALUE,            // 关闭输出
                         1
                         );

    TIMER_A_stop (HAL_SCM_NEG_PUMP_PWM_BASE);                       // Timer STOP
}

