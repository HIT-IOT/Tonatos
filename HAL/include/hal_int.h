/******************************************************************************
  Filename:     hal_int.h

  Description:  HAL interrupt control header file

******************************************************************************/


#ifndef HAL_INT_H
#define HAL_INT_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * INCLUDES
 */
#include <types.h>
#include <intrinsics.h>

/******************************************************************************
 * MACROS
 */


// Use the macros below to reduce function call overhead for common
// global interrupt control functions



#define HAL_INT_ON(x)      st( __enable_interrupt(); )
#define HAL_INT_OFF(x)     st( __disable_interrupt(); )
#define HAL_INT_LOCK(x)    st( (x) = __get_interrupt_state(); \
                               __disable_interrupt(); )
#define HAL_INT_UNLOCK(x)  st( __set_interrupt_state(x); )



/******************************************************************************
 * GLOBAL FUNCTIONS
 */

void   halIntOn(void);
void   halIntOff(void);
uint16 halIntLock(void);
void   halIntUnlock(uint16 key);


#ifdef  __cplusplus
}
#endif


#endif
