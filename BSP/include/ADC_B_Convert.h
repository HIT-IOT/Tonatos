
#ifndef _ADC_CONVERT
#define _ADC_CONVERT

#include <types.h>
#include <adc12_b.h>
extern void Setup_ADC12 (void)  ;                                       // 总初始化ADC12模块
extern void ADC_convert_sequence (uint16_t _start_position);            // 开始ADC转换序列


#endif
