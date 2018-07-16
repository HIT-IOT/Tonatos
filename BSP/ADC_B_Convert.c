
/*
    ADC转换程序
    本模块仅为官方ADC驱动的 部分功能扩展，不包含所有功能

    AD转换中断入口仅有一个，但AD功能有多个，分别处于不同模块。因此构建本模块，专用于统一的AD转换
    采集完毕后即返回，不包含数据处理

    本模块使用循环睡眠等待（为节能），可能影响系统复杂的睡眠管理。先这么用，做操作系统时再改


针对2.3版节点：
    AD通道0~7中包含电压检测，24~31包含CPU内部AD通道（如温度、VCC电压等），统一为64周期采样时间，因为电压测量输入阻抗200K，需要50us的稳定时间；
    AD通道8~23中包含电流测量、各种传感器。为提高电流测量精度，现采样时间为32周期。如需高速传感器数据，该采样时间可缩短，但电流测量精度会下降。


    本模块还包含AD转换的总初始化。
    通道和中断的配置在各个分模块中。

*/


//  系统区

#include <ADC_B_Convert.h>
#include <types.h>
#include <defs.h>
#include <adc12_b.h>

static bool ADC_convert_OVER = FALSE;                       // AD转换完毕标志

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

void Setup_ADC12 (void)                                     // 总初始化ADC12模块
{
// *************************    初始化ADC12    ************************************
//  说明：IEA默认手动开始AD转换，而不是自动开始。AD转换时CPU待机LPM3等待转换完成。低功耗模式，牺牲转换速度以节能。不使用内部AD通道

    //Initialize the ADC12B Module
        /*
         * Base address of ADC12B Module
         * Use SC bit as sample/hold signal to start conversion
         * USE MODOSC 5MHZ Digital Oscillator as clock source
         * Use clock divider/pre-divider of 5
         * Not use internal channel
         */
        ADC12_B_initialize(ADC12_B_BASE,
                     ADC12_B_SAMPLEHOLDSOURCE_SC,               // 程序控制启停
                     ADC12_B_CLOCKSOURCE_ADC12OSC,              // ADC12OSC时钟源
                     ADC12_B_CLOCKDIVIDER_5,                    // 分频5, 4.8/5 ≈ 960K
                     ADC12_B_CLOCKPREDIVIDER__1,
                     ADC12_B_NOINTCH);                          // 无内部通道

        //Enable the ADC12B module
        ADC12_B_enable(ADC12_B_BASE);

        /*
         * Base address of ADC12B Module
         * For memory buffers 0-7 sample/hold for 64 clock cycles
         * For memory buffers 8-15 sample/hold for 8 clock cycles
         * a sequenced and/or repeated conversion mode
         */
        ADC12_B_setupSamplingTimer(ADC12_B_BASE,
                                   ADC12_B_CYCLEHOLD_64_CYCLES, // 通道0~7、24~31，采样时间64周期，因为电压测量输入阻抗200K，需要50us的稳定时间
                                   ADC12_B_CYCLEHOLD_32_CYCLES, // 通道8~23，采样时间32周期。电流采样需要20us以上的稳定时间。最少为16周期
                                   ADC12_B_MULTIPLESAMPLESENABLE);

        /*
         * Base address of ADC12B Module
         * granting additional power savings
         */
        ADC12_B_setAdcPowerMode(ADC12_B_BASE,
                                ADC12_B_LOWPOWERMODE);          // 低功耗模式
}



void ADC_convert_sequence (uint16_t _start_position)            // 开始ADC转换序列
{
    ADC_convert_OVER = FALSE;                                                       // 转换未完毕标志
    ADC12_B_startConversion(ADC12_B_BASE, _start_position, ADC12_B_SEQOFCHANNELS);  // 开始
    while (ADC_convert_OVER == FALSE)   __low_power_mode_3();                       // LPM3深度睡眠
}



/***********************    中断  ************************************************************/
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

    ADC12IFGR0 = 0;                                 // 清空MEM0 - 15 标志
    ADC12IV = 0;                                    // 清空所有AD中断标志                               注：需考虑对其它AD通道的影响
    ADC_convert_OVER = TRUE;                        // 转换完毕
    __low_power_mode_off_on_exit();                 // 唤醒CPU
}
