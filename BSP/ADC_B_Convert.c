
/*
    ADCת������
    ��ģ���Ϊ�ٷ�ADC������ ���ֹ�����չ�����������й���

    ADת���ж���ڽ���һ������AD�����ж�����ֱ��ڲ�ͬģ�顣��˹�����ģ�飬ר����ͳһ��ADת��
    �ɼ���Ϻ󼴷��أ����������ݴ���

    ��ģ��ʹ��ѭ��˯�ߵȴ���Ϊ���ܣ�������Ӱ��ϵͳ���ӵ�˯�߹�������ô�ã�������ϵͳʱ�ٸ�


���2.3��ڵ㣺
    ADͨ��0~7�а�����ѹ��⣬24~31����CPU�ڲ�ADͨ�������¶ȡ�VCC��ѹ�ȣ���ͳһΪ64���ڲ���ʱ�䣬��Ϊ��ѹ���������迹200K����Ҫ50us���ȶ�ʱ�䣻
    ADͨ��8~23�а����������������ִ�������Ϊ��ߵ����������ȣ��ֲ���ʱ��Ϊ32���ڡ�������ٴ��������ݣ��ò���ʱ������̣��������������Ȼ��½���


    ��ģ�黹����ADת�����ܳ�ʼ����
    ͨ�����жϵ������ڸ�����ģ���С�

*/


//  ϵͳ��

#include <ADC_B_Convert.h>
#include <types.h>
#include <defs.h>
#include <adc12_b.h>

static bool ADC_convert_OVER = FALSE;                       // ADת����ϱ�־

bool ADC12_B_initialize(uint16_t baseAddress,
                  uint16_t sampleHoldSignalSourceSelect,
                  uint8_t clockSourceSelect,
                  uint16_t clockSourceDivider,
                  uint16_t clockSourcePredivider,
                  uint16_t internalChannelMap)
{
        ADC12_B_initParam param = { 0 };

        param.sampleHoldSignalSourceSelect = sampleHoldSignalSourceSelect;
        param.clockSourceSelect = clockSourceSelect;
        param.clockSourceDivider = clockSourceDivider;
        param.clockSourcePredivider = clockSourcePredivider;
        param.internalChannelMap = internalChannelMap;

        return ADC12_B_init(baseAddress, &param);
}

void ADC12_B_memoryConfigure(uint16_t baseAddress,
                             uint8_t memoryBufferControlIndex,
                             uint8_t inputSourceSelect,
                             uint16_t refVoltageSourceSelect,
                             uint16_t endOfSequence,
                             uint16_t windowComparatorSelect,
                             uint16_t differentialModeSelect
                             )
{
        ADC12_B_configureMemoryParam param = { 0 };

        param.memoryBufferControlIndex = memoryBufferControlIndex;
        param.inputSourceSelect = inputSourceSelect;
        param.refVoltageSourceSelect = refVoltageSourceSelect;
        param.endOfSequence = endOfSequence;
        param.windowComparatorSelect = windowComparatorSelect;
        param.differentialModeSelect = differentialModeSelect;

        ADC12_B_configureMemory(baseAddress, &param);
}

void Setup_ADC12 (void)                                     // �ܳ�ʼ��ADC12ģ��
{
// *************************    ��ʼ��ADC12    ************************************
//  ˵����IEAĬ���ֶ���ʼADת�����������Զ���ʼ��ADת��ʱCPU����LPM3�ȴ�ת����ɡ��͹���ģʽ������ת���ٶ��Խ��ܡ���ʹ���ڲ�ADͨ��

    //Initialize the ADC12B Module
        /*
         * Base address of ADC12B Module
         * Use SC bit as sample/hold signal to start conversion
         * USE MODOSC 5MHZ Digital Oscillator as clock source
         * Use clock divider/pre-divider of 5
         * Not use internal channel
         */
        ADC12_B_initialize(ADC12_B_BASE,
                     ADC12_B_SAMPLEHOLDSOURCE_SC,               // ���������ͣ
                     ADC12_B_CLOCKSOURCE_ADC12OSC,              // ADC12OSCʱ��Դ
                     ADC12_B_CLOCKDIVIDER_5,                    // ��Ƶ5, 4.8/5 �� 960K
                     ADC12_B_CLOCKPREDIVIDER__1,
                     ADC12_B_NOINTCH);                          // ���ڲ�ͨ��

        //Enable the ADC12B module
        ADC12_B_enable(ADC12_B_BASE);

        /*
         * Base address of ADC12B Module
         * For memory buffers 0-7 sample/hold for 64 clock cycles
         * For memory buffers 8-15 sample/hold for 8 clock cycles
         * a sequenced and/or repeated conversion mode
         */
        ADC12_B_setupSamplingTimer(ADC12_B_BASE,
                                   ADC12_B_CYCLEHOLD_64_CYCLES, // ͨ��0~7��24~31������ʱ��64���ڣ���Ϊ��ѹ���������迹200K����Ҫ50us���ȶ�ʱ��
                                   ADC12_B_CYCLEHOLD_32_CYCLES, // ͨ��8~23������ʱ��32���ڡ�����������Ҫ20us���ϵ��ȶ�ʱ�䡣����Ϊ16����
                                   ADC12_B_MULTIPLESAMPLESENABLE);

        /*
         * Base address of ADC12B Module
         * granting additional power savings
         */
        ADC12_B_setAdcPowerMode(ADC12_B_BASE,
                                ADC12_B_LOWPOWERMODE);          // �͹���ģʽ
}



void ADC_convert_sequence (uint16_t _start_position)            // ��ʼADCת������
{
    ADC_convert_OVER = FALSE;                                                       // ת��δ��ϱ�־
    ADC12_B_startConversion(ADC12_B_BASE, _start_position, ADC12_B_SEQOFCHANNELS);  // ��ʼ
    while (ADC_convert_OVER == FALSE)   __low_power_mode_3();                       // LPM3���˯��
}



/***********************    �ж�  ************************************************************/
//  ADC12

HAL_ISR_FUNCTION(ADC12_ISR,ADC12)
{
/*
        switch (__even_in_range(ADC12IV, 76)) {
        case  0: break;                         // Vector  0:  No interrupt
        case  2: break;                         // Vector  2:  ADC12BMEMx Overflow
        case  4: break;                         // Vector  4:  Conversion time overflow
        case  6: break;                         // Vector  6:  ADC12BHI
        case  8: break;                         // Vector  8:  ADC12BLO
        case 10: break;                         // Vector 10:  ADC12BIN
        case 12: break;                              // Vector 12:  ADC12BMEM0 Interrupt
        case 14: break;                                 // Vector 14:  ADC12BMEM1
        case 16: break;                                 // Vector 16:  ADC12BMEM2
        case 18: break;                                 // Vector 18:  ADC12BMEM3
        case 20: break;                                 // Vector 20:  ADC12BMEM4
        case 22: break;                                 // Vector 22:  ADC12BMEM5
        case 24: break;                                 // Vector 24:  ADC12BMEM6
        case 26: break;                                 // Vector 26:  ADC12BMEM7
        case 28: break;                                 // Vector 28:  ADC12BMEM8
        case 30: break;                                 // Vector 30:  ADC12BMEM9
        case 32: break;                                 // Vector 32:  ADC12BMEM10
        case 34: break;                                 // Vector 34:  ADC12BMEM11
        case 36: break;                                 // Vector 36:  ADC12BMEM12
        case 38: break;                                 // Vector 38:  ADC12BMEM13
        case 40: break;                                 // Vector 40:  ADC12BMEM14
        case 42: break;                                 // Vector 42:  ADC12BMEM15
        case 44: break;                                 // Vector 44:  ADC12BMEM16
        case 46: break;                                 // Vector 46:  ADC12BMEM17
        case 48: break;                                 // Vector 48:  ADC12BMEM18
        case 50: break;                                 // Vector 50:  ADC12BMEM19
        case 52: break;                                 // Vector 52:  ADC12BMEM20
        case 54: break;                                 // Vector 54:  ADC12BMEM21
        case 56: break;                                 // Vector 56:  ADC12BMEM22
        case 58: break;                                 // Vector 58:  ADC12BMEM23
        case 60: break;                                 // Vector 60:  ADC12BMEM24
        case 62: break;                                 // Vector 62:  ADC12BMEM25
        case 64: break;                                 // Vector 64:  ADC12BMEM26
        case 66: break;                                 // Vector 66:  ADC12BMEM27
        case 68: break;                                 // Vector 68:  ADC12BMEM28
        case 70: break;                                 // Vector 70:  ADC12BMEM29
        case 72: break;                                 // Vector 72:  ADC12BMEM30
        case 74: break;                                 // Vector 74:  ADC12BMEM31
        case 76: break;                                 // Vector 76:  ADC12BRDY
        default: break;
        }
*/

    ADC12IFGR0 = 0;                                 // ���MEM0 - 15 ��־
    ADC12IV = 0;                                    // �������AD�жϱ�־                               ע���迼�Ƕ�����ADͨ����Ӱ��
    ADC_convert_OVER = TRUE;                        // ת�����
    __low_power_mode_off_on_exit();                 // ����CPU
}
