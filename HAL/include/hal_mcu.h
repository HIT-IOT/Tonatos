/***********************************************************************************
  Filename:     hal_mcu.h

  Description:  hal mcu library header file

***********************************************************************************/

#ifndef HAL_MCU_H
#define HAL_MCU_H

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************
* INCLUDES
*/
#include <types.h>
#include <intrinsics.h>

/***********************************************************************************
 * CONSTANTS AND DEFINES
 */
enum {                  // Input values to halMcuSetLowPowerMode()
    HAL_MCU_LPM_0,
    HAL_MCU_LPM_1,
    HAL_MCU_LPM_2,
    HAL_MCU_LPM_3,
    HAL_MCU_LPM_4
};


enum {                  // Return values of halMcuGetResetCause()
    HAL_MCU_RESET_CAUSE_POR,
    HAL_MCU_RESET_CAUSE_EXTERNAL,
    HAL_MCU_RESET_CAUSE_WATCHDOG
};



#define HAL_ENABLE_INTERRUPTS()         asm("eint")
#define HAL_DISABLE_INTERRUPTS()        st( asm("dint"); asm("nop"); )
#define HAL_INTERRUPTS_ARE_ENABLED()    (__get_SR_register() & GIE)

typedef unsigned short halIntState_t;
#define HAL_ENTER_CRITICAL_SECTION(x)   st( x = __get_interrupt_state();  HAL_DISABLE_INTERRUPTS(); )
#define HAL_EXIT_CRITICAL_SECTION(x)    st( __set_interrupt_state(x); )
#define HAL_CRITICAL_STATEMENT(x)       st( halIntState_t s; HAL_ENTER_CRITICAL_SECTION(s); x; HAL_EXIT_CRITICAL_SECTION(s); )


/***********************************************************************************
 * GLOBAL FUNCTIONS
 */

void halMcuInit(void);
void halMcuWaitUs(uint16 usec);
void halMcuWaitMs(uint16 msec);
void halMcuSetLowPowerMode(uint8 mode);
void halMcuReset(void);
uint8 halMcuGetResetCause(void);


/* ------------------------------------------------------------------------------------------------
 *                                        Reset Macro
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_SYSTEM_RESET()              st( WDTCTL = ~WDTPW; )  /* force immediate system reset */


#ifdef  __cplusplus
}
#endif

/**********************************************************************************/
#endif
