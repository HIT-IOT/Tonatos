
/*
    能量感知与控制
    2018.2.19

    本部分与硬件密切相关，更改任何IO需要更改宏定义和代码

注意：
    第一次的电流测量结果是无效的

注意：
    当系统电压高于2.1V时，LDO恒定输出2.12V电压，此时AD工作稳定；
    当系统电压低于2.1V时，由于参考电压随之降低，测量的AD值将不再准确：
        主电容电压测量值，将恒定为2.12V

    因此当测量到系统电压低于2.15V时，其测量的AD值将不再准确。此时如要获取精确的AD值，需开启内部参考源（此程序不包含）
    需注意当系统电压低于2.05V时，节点将关闭。

注意：
    当连接编程口并供电时，测量的AD值将不再准确！！！
    调试程序时，如果节点接有大功率外接电源且不为2.9-3.3V，最好断掉该外接电源。


注意：
    电容C1、C2电压低于1V时测得的结果是不准确的，偏差很大。
    开启C2负压泵可有效提高超级电容放电能力。两种情况开启：设定RTC唤醒时、MCU直接控制C2放电时

注意：
    C2的充电程序和C2的放电负压泵程序不可同时调用，必须先关掉一个再开启另一个，否则会引起混乱。（程序内已处理）
    系统库函数timer_a.c有BUG，已经更正。因此不要更改该修正过的库函数

注意：
    电流检测部分，电容复位后初始值采样时间为2毫秒，该时间内CPU待机，可以优化


//  ADC12   12bits ADC
    本部分设定：
    参考电压为AVCC和AGND，不使用参考电压，不使用差分，不使用窗口比较
    数据右对齐，无符号整形
    由于需要控制IO，需要软件触发
    采样时间及RC取值通过示波器实测


本模块电压和电流分开测量，互不影响
    使用前需要初始化。
    未测量的通道其值和状态不受影响

    电压测量初始化后不可关闭，因为不占用资源，也无法关闭
        电压感知可随时调用，执行时间大约250us。
        返回单位uV

    电流测量可开启可关闭。关闭后电流测量模块与系统断开，不再耗电，也没有任何关联。
        电流感知需记录调用间隔，测量结果表示该段时间内流过的电量。
        由于不同量程放大倍数不同，该处仅返回电压值。返回单位uV
        假设采集间隔1秒，返回值500000，放大倍数200，采样电阻10
            则平均电流为 500000/(200*10)=250uA
        当采样间隔小于200毫秒时，需考虑电容复位导致的误差，详见调试手册。

关于通道抑制：
    IEA的电流测量 使用两通道实现 宽量程测量
    有时电流过大，低量程通道持续溢出，其值没有任何作用，CPU还需要不断复位电容
        这时可以抑制相应通道。抑制后，该通道测量值无效（约为0），而且无需复位电容。（基本不增加能耗）
        释放后该通道恢复正常
    注意电流感知关闭时通道自动释放，下次开启后需要重新抑制。
    如果两路全部抑制，还不如直接关掉，更省电

    （本质上抑制就是一直开启电容复位）

*/

//  系统区


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

