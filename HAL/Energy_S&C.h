
/*
    ������֪�����
    2018.2.19

    ��������Ӳ��������أ������κ�IO��Ҫ���ĺ궨��ʹ���

ע�⣺
    ��һ�εĵ��������������Ч��

ע�⣺
    ��ϵͳ��ѹ����2.1Vʱ��LDO�㶨���2.12V��ѹ����ʱAD�����ȶ���
    ��ϵͳ��ѹ����2.1Vʱ�����ڲο���ѹ��֮���ͣ�������ADֵ������׼ȷ��
        �����ݵ�ѹ����ֵ�����㶨Ϊ2.12V

    ��˵�������ϵͳ��ѹ����2.15Vʱ���������ADֵ������׼ȷ����ʱ��Ҫ��ȡ��ȷ��ADֵ���迪���ڲ��ο�Դ���˳��򲻰�����
    ��ע�⵱ϵͳ��ѹ����2.05Vʱ���ڵ㽫�رա�

ע�⣺
    �����ӱ�̿ڲ�����ʱ��������ADֵ������׼ȷ������
    ���Գ���ʱ������ڵ���д�����ӵ�Դ�Ҳ�Ϊ2.9-3.3V����öϵ�����ӵ�Դ��


ע�⣺
    ����C1��C2��ѹ����1Vʱ��õĽ���ǲ�׼ȷ�ģ�ƫ��ܴ�
    ����C2��ѹ�ÿ���Ч��߳������ݷŵ���������������������趨RTC����ʱ��MCUֱ�ӿ���C2�ŵ�ʱ

ע�⣺
    C2�ĳ������C2�ķŵ縺ѹ�ó��򲻿�ͬʱ���ã������ȹص�һ���ٿ�����һ���������������ҡ����������Ѵ���
    ϵͳ�⺯��timer_a.c��BUG���Ѿ���������˲�Ҫ���ĸ��������Ŀ⺯��

ע�⣺
    ������ⲿ�֣����ݸ�λ���ʼֵ����ʱ��Ϊ2���룬��ʱ����CPU�����������Ż�


//  ADC12   12bits ADC
    �������趨��
    �ο���ѹΪAVCC��AGND����ʹ�òο���ѹ����ʹ�ò�֣���ʹ�ô��ڱȽ�
    �����Ҷ��룬�޷�������
    ������Ҫ����IO����Ҫ�������
    ����ʱ�估RCȡֵͨ��ʾ����ʵ��


��ģ���ѹ�͵����ֿ�����������Ӱ��
    ʹ��ǰ��Ҫ��ʼ����
    δ������ͨ����ֵ��״̬����Ӱ��

    ��ѹ������ʼ���󲻿ɹرգ���Ϊ��ռ����Դ��Ҳ�޷��ر�
        ��ѹ��֪����ʱ���ã�ִ��ʱ���Լ250us��
        ���ص�λuV

    ���������ɿ����ɹرա��رպ��������ģ����ϵͳ�Ͽ������ٺĵ磬Ҳû���κι�����
        ������֪���¼���ü�������������ʾ�ö�ʱ���������ĵ�����
        ���ڲ�ͬ���̷Ŵ�����ͬ���ô������ص�ѹֵ�����ص�λuV
        ����ɼ����1�룬����ֵ500000���Ŵ���200����������10
            ��ƽ������Ϊ 500000/(200*10)=250uA
        ���������С��200����ʱ���迼�ǵ��ݸ�λ���µ�����������ֲᡣ

����ͨ�����ƣ�
    IEA�ĵ������� ʹ����ͨ��ʵ�� �����̲���
    ��ʱ�������󣬵�����ͨ�������������ֵû���κ����ã�CPU����Ҫ���ϸ�λ����
        ��ʱ����������Ӧͨ�������ƺ󣬸�ͨ������ֵ��Ч��ԼΪ0�����������踴λ���ݡ��������������ܺģ�
        �ͷź��ͨ���ָ�����
    ע�������֪�ر�ʱͨ���Զ��ͷţ��´ο�������Ҫ�������ơ�
    �����·ȫ�����ƣ�������ֱ�ӹص�����ʡ��

    �����������ƾ���һֱ�������ݸ�λ��

*/

//  ϵͳ��


#include <ADC_B_Convert.h>
#include <Energy_S&C.h>
#include <bsp_timer_a.h>
#include <RTC.h>
#include <hal_mcu.h>
#include <hal.h>
#include <macros.h>
#include <msp430.h>
#include <bsp_timer_a.h>

#define     _Vcc_Voltage            2120                // VCC��ѹ����λ��mV��

Energy_info     Energy_measure_result;                  // �����������

static bool Energy_volt_enable = false;             // ��ѹ����ʹ��
static bool Energy_current_enable = false;          // ��������ʹ��
static bool Energy_current_1st_Inhibitor = false;   // ������һͨ��������
static bool Energy_current_2nd_Inhibitor = false;   // �����ڶ�ͨ��������


static uint16   _init_PS_Harv_1st, _init_PS_Harv_2nd;   // �ڲ�����������������ʼ��





void Energy_Volt_init (void)                                            // ����-��ѹ ��ʼ��
{
    MCU_IO_PERIPHERAL_3 (HAL_ADC_PS_VOLT_PORT, HAL_ADC_PS_VOLT_PIN, MCU_IO_DIR_INPUT);   // �����ݵ�ѹ���
    MCU_IO_PERIPHERAL_3 (HAL_ADC_SCM_VOLT_PORT, HAL_ADC_SCM_VOLT_PIN, MCU_IO_DIR_INPUT); // �����ݵ�ѹ���


//  ------------------------    ��ʼ��ADת��ͨ��   --------------------------------

    //Configure Memory Buffer
    /*
        MEM0��MEM1Ϊ��ѹ��⡣�ο�Դ��ΪAVCC��AGND�������Ǵ��ںͲ��
        MEM0�� HAL_ADC_PS_VOLT
        MEM1�� HAL_ADC_SCM_VOLT              ��ɺ������ж�
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


//  ------------------------    ͨ����ʼ����� --------------------------------

        ADC12_B_clearInterrupt(ADC12_B_BASE,
                               0,                                               // 0�Σ�MEM0~MEM15
                               ADC12_B_IFG1                                     // ��� MEM1 ��־
                               );

        //Enable memory buffer 1 interrupt
        ADC12_B_enableInterrupt(ADC12_B_BASE,
                                ADC12_B_IE1,                                    // MEM0~MEM15       ʹ�� MEM1 �ж�
                                0,                                              // MEM16~MEM31
                                0);                                             // ����

//  ----------------------------    ��ʼ������   --------------------------------

    Energy_measure_result.Pri_cap_volt = 0;
    Energy_measure_result.Sec_cap_volt = 0;

    Energy_volt_enable = true;                                                  // ��ѹ����ʹ��
}



void Energy_Current_init (void)                                     // ����-���� ��ʼ��
{
    MCU_IO_PERIPHERAL_3 (HAL_ADC_PS_Harv_1st_PORT, HAL_ADC_PS_Harv_1st_PIN, MCU_IO_DIR_INPUT);  // �ջ��ɼ�⣨����1��
    MCU_IO_PERIPHERAL_3 (HAL_ADC_PS_Harv_2nd_PORT, HAL_ADC_PS_Harv_2nd_PIN, MCU_IO_DIR_INPUT);  // �ջ��ɼ�⣨����2��


//  ------------------------    ��ʼ��ADת��ͨ��   --------------------------------

    //Configure Memory Buffer
    /*
        MEM2��MEM3Ϊ������⡣�ο�Դ��ΪAVCC��AGND�������Ǵ��ںͲ��
        MEM2�� HAL_ADC_PS_Harv_1st
        MEM3�� HAL_ADC_PS_Harv_2nd           ��ɺ������ж�
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


//  ------------------------    ͨ����ʼ����� --------------------------------

        ADC12_B_clearInterrupt(ADC12_B_BASE,
                               0,                                               // 0�Σ�MEM0~MEM15
                               ADC12_B_IFG3                                     // ��� MEM3 ��־
                               );

        //Enable memory buffer 3 interrupt
        ADC12_B_enableInterrupt(ADC12_B_BASE,
                                ADC12_B_IE3,                                    // MEM0~MEM15       ʹ�� MEM3 �ж�
                                0,                                              // MEM16~MEM31
                                0);                                             // ����


//  ----------------------------    ��ʼ��Ӳ��   --------------------------------

    Current_Amp_EN (CHARGE_PUMP_CLK_512);                                       // ����������������ѹ�� 512Hz


//  ----------------------------    ��ʼ������   --------------------------------

    _init_PS_Harv_1st = 0;
    _init_PS_Harv_2nd = 0;
    Energy_measure_result.Harv_1st_charge = 0;
    Energy_measure_result.Harv_2nd_charge = 0;

    Energy_current_1st_Inhibitor = false;                                       // ������һͨ����������Ч
    Energy_current_2nd_Inhibitor = false;                                       // �����ڶ�ͨ����������Ч
    Amp_1st_clr_dis();
    Amp_2nd_clr_dis();                                                          // �ſ����ݸ�λ

    Energy_current_enable = true;                                               // ��������ʹ��
}


void Energy_Current_Disable (void)                                              // �رյ�������
{
    Energy_current_1st_Inhibitor = false;                                       // ������һͨ����������Ч
    Energy_current_2nd_Inhibitor = false;                                       // �����ڶ�ͨ����������Ч

    Amp_1st_clr_dis();
    Amp_2nd_clr_dis();                                                          // �ſ����ݸ�λ

    Current_Amp_DIS ();                                                         // ��������ֹͣ
    Energy_current_enable = false;                                              // ����ʧЧ
}





uint8 Energy_Volt_Sense (uint8 _sense_mode)                             // ��ѹ��֪
{
    if (!Energy_volt_enable)    return(false);                                  // δ��ʼ�����˳�
    if (_sense_mode >= 0x04)    return(false);                                  // ���������˳�
    if (_sense_mode == 0)       return(false);                                  // ���������˳�

    uint32 _convertA, _convertB;

    if ((_sense_mode & Energy_Voltage_C1) != 0)     Meas_C1_en();               // �Ƿ����C1
    if ((_sense_mode & Energy_Voltage_C2) != 0)     Meas_C2_en();               // �Ƿ����C2
    halMcuWaitUs (25);                                                          // �ȴ�������ѹ

    ADC_convert_sequence (ADC12_B_START_AT_ADC12MEM0);                          // ������ѹ
    Meas_C1_dis();
    Meas_C2_dis();


    if ((_sense_mode & Energy_Voltage_C1) != 0)                                 // �Ƿ����C1
    {
        _convertA = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_0);         // HAL_ADC_PS_VOLT      ���� = ����(200K+100K)/100K = 3
        _convertA = (_convertA * _Vcc_Voltage * 375)>>9;                        // ��λ��΢��            = _Vcc_Voltage*3*1000/4096
        Energy_measure_result.Pri_cap_volt = _convertA;
    }

    if ((_sense_mode & Energy_Voltage_C2) != 0)                                 // �Ƿ����C2
    {
        _convertB = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_1);         // HAL_ADC_SCM_VOLT     ���� = ����(200K+100K)/100K = 3
        _convertB = (_convertB * _Vcc_Voltage * 375)>>9;                        // ��λ��΢��            = _Vcc_Voltage*3*1000/4096
        Energy_measure_result.Sec_cap_volt = _convertB;
    }

    return (true);
}



uint8 Energy_Current_Sense (uint8 _sense_mode)                          // ������֪
{
    if (!Energy_current_enable) return(false);                                  // δ��ʼ�����˳�
    if (_sense_mode >= 0x40)    return(false);                                  // ���������˳�
    if (_sense_mode < 0x04)     return(false);                                  // ���������˳�


    uint32 _convertA, _convertB;


    ADC_convert_sequence (ADC12_B_START_AT_ADC12MEM2);                          // ��������

    if ((_sense_mode & (Energy_Sample_1st + Energy_Sample_and_Clear_1st)) != 0) // �Ƿ��������1
    {
        if ((_sense_mode & Energy_Sample_and_Clear_1st) != 0)                   // �Ƿ�������㡣ע��δ��������ģ��ʱ�ò������˷�����
            Amp_1st_clr_en();                                                   // ������������29us
        //  ------------
        _convertA = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_2);         // HAL_ADC_PS_Harv_1st
        if (_convertA > _init_PS_Harv_1st)      _convertA -= _init_PS_Harv_1st; // ����ʼֵ
        else _convertA = 0;
        _convertA = (_convertA * _Vcc_Voltage * 125) >>9;                       // ��λ��΢��            = _Vcc_Voltage*1000/4096
        Energy_measure_result.Harv_1st_charge = _convertA;
        //  ------------    4M��Ƶ������ʱ��29us
        if (!Energy_current_1st_Inhibitor)  Amp_1st_clr_dis();                  // ���ͨ��������ֹͣ������������
    }

    if ((_sense_mode & (Energy_Sample_2nd + Energy_Sample_and_Clear_2nd)) != 0) // �Ƿ��������2
    {
        if ((_sense_mode & Energy_Sample_and_Clear_2nd) != 0)                   // �Ƿ��������
            Amp_2nd_clr_en();                                                   // ������������29us
        //  ------------
        _convertB = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_3);         // HAL_ADC_PS_Harv_2nd
        if (_convertB > _init_PS_Harv_2nd)      _convertB -= _init_PS_Harv_2nd; // ����ʼֵ
        else _convertB = 0;
        _convertB = (_convertB * _Vcc_Voltage * 125) >>9;                       // ��λ��΢��            = _Vcc_Voltage*1000/4096
        Energy_measure_result.Harv_2nd_charge = _convertB;
        //  ------------    4M��Ƶ������ʱ��29us
        if (!Energy_current_2nd_Inhibitor)  Amp_2nd_clr_dis();                  // ���ͨ��������ֹͣ������������
    }


    if (_sense_mode >= Energy_Sample_and_Clear_1st)
    {
        if ((((_sense_mode & Energy_Sample_and_Clear_1st) != 0) && (!Energy_current_1st_Inhibitor))||(((_sense_mode & Energy_Sample_and_Clear_2nd) != 0) && (!Energy_current_2nd_Inhibitor)))   // �Ƿ���Ҫ2ms��ʱ
        {
            // �˷ŷ�Ӧ��Ҫʱ�䣡���� �˴�Ӧ���������ʱ�䣬�����ֵ�����������       ****************************************
            // ���ߵȴ�2���룬��ʱʹ��TA3.1
            uint16 _delaytime;
            _delaytime = GetSystemClock() + (Standard_1ms *2);                      // ��΢����2ms
            if (_delaytime >= RTC_UP_BOUND)     _delaytime -= RTC_UP_BOUND;

            TIMER_A_clearCaptureCompareInterruptFlag (TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);    // ���ж�
            TIMER_A_initCompare (TIMER_A3_BASE,
                                 TIMER_A_CAPTURECOMPARE_REGISTER_1,
                                 TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,
                                 TIMER_A_OUTPUTMODE_OUTBITVALUE,
                                 _delaytime     );                                  // ����TA3.1�Ƚ������ж�
            do
            {   __low_power_mode_3();                                               // LPM3
            }while (!TIMER_A_getCaptureCompareInterruptStatus (TIMER_A3_BASE,
                                                                TIMER_A_CAPTURECOMPARE_REGISTER_1,
                                                                TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG)
                    );                                                              // ���ѵ�ʱ�䣬����
        }   // -----------------------------------------------------------------------------------------------------------------

        ADC_convert_sequence (ADC12_B_START_AT_ADC12MEM2);                      // ������ʼ����

        if ((_sense_mode & Energy_Sample_and_Clear_1st) != 0)   _init_PS_Harv_1st = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_2);     // ��¼������ʼֵ����ֵ����
        if ((_sense_mode & Energy_Sample_and_Clear_2nd) != 0)   _init_PS_Harv_2nd = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_3);     // ��¼������ʼֵ����ֵ����
    }

    return (true);
}



void Inhibit_1st_Current_Channel (void)                                     // ���Ƶ�һ����ͨ��
{
    if (!Energy_current_enable) return;                                         // δ��ʼ�����˳�

    Energy_current_1st_Inhibitor = true;
    Amp_1st_clr_en();
}


void Inhibit_2nd_Current_Channel (void)                                     // ���Ƶڶ�����ͨ��
{
    if (!Energy_current_enable) return;                                         // δ��ʼ�����˳�

    Energy_current_2nd_Inhibitor = true;
    Amp_2nd_clr_en();
}


void Release_1st_Current_Channel (void)                                     // �ͷŵ�һ����ͨ��
{
    if (!Energy_current_enable) return;                                         // δ��ʼ�����˳�

    Energy_current_1st_Inhibitor = false;
    Amp_1st_clr_dis();
}


void Release_2nd_Current_Channel (void)                                     // �ͷŵڶ�����ͨ��
{
    if (!Energy_current_enable) return;                                         // δ��ʼ�����˳�

    Energy_current_2nd_Inhibitor = false;
    Amp_2nd_clr_dis();
}





void Current_Amp_EN (uint16 value)                                              // ��������������ȡֵ0-7��Խ��Ƶ��Խ�ߡ�0Ϊ�ر�
{
    Meas_current_en();                                                          // �����˷Ź���

    MCU_IO_PERIPHERAL_1 (HAL_CRG_PUMP_CLK_PORT, HAL_CRG_PUMP_CLK_PIN, MCU_IO_DIR_OUTPUT);   // TA0.1��    PWM: SEL_1

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
    duty = value /2;                                                    // 50%ռ�ձ�
    if (value != 0)     value--;

    TIMER_A_generatePWM (HAL_CRG_PUMP_PWM_BASE,
                         TIMER_A_CLOCKSOURCE_ACLK,                      // ACLK, 32K
                         TIMER_A_CLOCKSOURCE_DIVIDER_1,                 // ACLK/1
                         value,                                         // ����
                         HAL_CRG_PUMP_PWM_CNL,
                         TIMER_A_OUTPUTMODE_SET_RESET,                  // �͵�ƽ����
                         duty                                           // 50%
                         );

    TIMER_A_startCounter (HAL_CRG_PUMP_PWM_BASE,
                          TIMER_A_UP_MODE                               // UP ģʽ����
                          );
}


void Current_Amp_DIS (void)                                                 // ��������ֹͣ
{
    TIMER_A_generatePWM (HAL_CRG_PUMP_PWM_BASE,
                         TIMER_A_CLOCKSOURCE_ACLK,
                         TIMER_A_CLOCKSOURCE_DIVIDER_1,
                         0,
                         HAL_CRG_PUMP_PWM_CNL,
                         TIMER_A_OUTPUTMODE_OUTBITVALUE,                // �ر����
                         1
                         );

    TIMER_A_stop (HAL_CRG_PUMP_PWM_BASE);                               // Timer STOP

    Meas_current_dis ();                                                // �ر��˷Ź���
}





void C2_Charge_EN (uint16 value)                                                // �򳬼����ݳ�硣ȡֵ0-1000��ԽСԽ�죬1000Ϊֹͣ��硣����ٶȼ���������1.4.1�������ԣ�
{
    MCU_IO_PERIPHERAL_2 (HAL_SCM_CRG_PORT, HAL_SCM_CRG_PIN, MCU_IO_DIR_OUTPUT); // TA1.2,   PWM: SEL_2

    C2_Discharge_Pump_DIS ();                                                   // ������C2_Discharge_Pump_ENͬ�ã�����

    TIMER_A_generatePWM (HAL_SCM_CRG_PWM_BASE,
                         TIMER_A_CLOCKSOURCE_ACLK,                  // ACLK, 32K
                         TIMER_A_CLOCKSOURCE_DIVIDER_1,             // ACLK/1
                         32768,
                         HAL_SCM_CRG_PWM_CNL,
                         TIMER_A_OUTPUTMODE_SET_RESET,              // �͵�ƽ����
                         1                                          // ʱ�� 1
                         );

    if (value >= 1000)  HAL_SCM_CRG_PEROID = 0;
    else    if (value < 2)  value = 2;
    else    HAL_SCM_CRG_PEROID = value;                             // ����Ϊ1ʱ�䵥λ���������ڣ�PFMģʽ

    TIMER_A_startCounter (HAL_SCM_CRG_PWM_BASE,
                          TIMER_A_UP_MODE                           // UP ģʽ����
                          );
}


void C2_Charge_DIS (void)                                                       // ֹͣ�򳬼����ݳ��
{
    TIMER_A_generatePWM (HAL_SCM_CRG_PWM_BASE,
                         TIMER_A_CLOCKSOURCE_ACLK,
                         TIMER_A_CLOCKSOURCE_DIVIDER_8,
                         0,
                         HAL_SCM_CRG_PWM_CNL,
                         TIMER_A_OUTPUTMODE_OUTBITVALUE,            // �ر����
                         1
                         );

    TIMER_A_stop (HAL_SCM_CRG_PWM_BASE);                            // Timer STOP
}



void C2_Discharge_Pump_EN (void)                                                // C2�ŵ�ÿ�����1KHz������3���Ӽ���
{
    MCU_IO_PERIPHERAL_2 (HAL_SCM_NEG_PUMP_PORT, HAL_SCM_NEG_PUMP_PIN, MCU_IO_DIR_OUTPUT);   // TA1.1,   PWM: SEL_2

    C2_Charge_DIS ();                                                           // ������C2_Charge_ENͬ�ã�����

    TIMER_A_generatePWM (HAL_SCM_NEG_PUMP_PWM_BASE,
                         TIMER_A_CLOCKSOURCE_ACLK,                  // ACLK, 32K
                         TIMER_A_CLOCKSOURCE_DIVIDER_1,             // ACLK/1
                         31,                                        // Ƶ��1KHz
                         HAL_SCM_NEG_PUMP_PWM_CNL,
                         TIMER_A_OUTPUTMODE_SET_RESET,              // �͵�ƽ����
                         16                                         // 50%
                         );

    TIMER_A_startCounter (HAL_SCM_NEG_PUMP_PWM_BASE,
                          TIMER_A_UP_MODE                           // UP ģʽ����
                          );
}


void C2_Discharge_Pump_DIS (void)                                               // C2�ŵ�ùر�
{
    TIMER_A_generatePWM (HAL_SCM_NEG_PUMP_PWM_BASE,
                         TIMER_A_CLOCKSOURCE_ACLK,
                         TIMER_A_CLOCKSOURCE_DIVIDER_1,
                         0,
                         HAL_SCM_NEG_PUMP_PWM_CNL,
                         TIMER_A_OUTPUTMODE_OUTBITVALUE,            // �ر����
                         1
                         );

    TIMER_A_stop (HAL_SCM_NEG_PUMP_PWM_BASE);                       // Timer STOP
}

