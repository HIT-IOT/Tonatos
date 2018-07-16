/***********************************************************************************
  Filename:     hal_mcu.c

  Description:  hal mcu library

  Confirm at:   2016.12.2


***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/
#include <types.h>
#include <defs.h>
#include <hal.h>
/***********************************************************************************
* @fn          halMcuWaitUs
*
* @brief       Busy wait function. Waits the specified number of microseconds. Use
*              assumptions about number of clock cycles needed for the various
*              instructions. The duration of one cycle depends on MCLK. In this HAL
*              , it is set to 8 MHz, thus 8 cycles per usec.
*
*              NB! This function is highly dependent on architecture and compiler!
*
* @param       uint16 usec - number of microseconds delay
*
* @return      none
*/
//#pragma optimize=none
void halMcuWaitUs(uint16 usec) // 5 cycles for calling
{
    // The least we can wait is 3 usec:
    // ~1 usec for call, 1 for first compare and 1 for return
    while(usec > 3)       // 2 cycles for compare
    {                       // 2 cycles for jump
        usec -= 1;        // 1 cycles for optimized decrement
    }
}                           // 4 cycles for returning


/***********************************************************************************
* @fn          halMcuWaitMs
*
* @brief       Busy wait function. Waits the specified number of milliseconds. Use
*              assumptions about number of clock cycles needed for the various
*              instructions.
*
*              NB! This function is highly dependent on architecture and compiler!
*
* @param       uint16 millisec - number of milliseconds delay
*
* @return      none
*/
//#pragma optimize=none
void halMcuWaitMs(uint16 msec)
{
    uint16  rounds = HAL_CPU_CLOCK_DELAY;

    while(msec-- > 0)
    {
        halMcuWaitUs(rounds);
    }
}

/***********************************************************************************
* @fn          halMcuReset
*
* @brief       MCU soft reset.
*
* @param       none
*
* @return      none
*/
void halMcuReset(void)
{
    void (*pf)(void)= 0;
    (*pf)();
}
