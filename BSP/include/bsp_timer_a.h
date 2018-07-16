/*
 * hal_timer_a.h
 *
 *  Created on: 2018Äê5ÔÂ31ÈÕ
 *      Author: carpela
 */

#ifndef HAL_INCLUDE_HAL_TIMER_A_H_
#define HAL_INCLUDE_HAL_TIMER_A_H_
#include <timer_a.h>

void TIMER_A_startCounter( uint32_t baseAddress,
                           uint16_t timerMode
                           );
void TIMER_A_generatePWM(  uint32_t baseAddress,
                           uint16_t clockSource,
                           uint16_t clockSourceDivider,
                           uint16_t timerPeriod,
                           uint16_t compareRegister,
                           uint16_t compareOutputMode,
                           uint16_t dutyCycle
                           );
#endif /* HAL_INCLUDE_HAL_TIMER_A_H_ */
