/*
 * macros.h
 *
 *  Created on: 2018Äê5ÔÂ30ÈÕ
 *      Author: carpela
 */

#ifndef INCLUDE_MACROS_H_
#define INCLUDE_MACROS_H_


/***********************************************************************************
* INCLUDES
*/

#include <types.h>
#include <defs.h>

/***********************************************************************************
* MACROS
*/

#define MCU_IO_DIR_INPUT    0
#define MCU_IO_DIR_OUTPUT   1

#define MCU_IO_PULLOFF      0
#define MCU_IO_TRISTATE     1
#define MCU_IO_PULLUP       2
#define MCU_IO_PULLDOWN     3

#define MCU_IO_DRIVE_WEAK   0               // Reduced drive strength
#define MCU_IO_DRIVE_FULL   1               // Full drive strength



//----------------------------------------------------------------------------------
//  Macros for simple configuration of IO pins on MSP430
//----------------------------------------------------------------------------------
#define MCU_IO_UNLOCK()                     st( PM5CTL0 &= ~LOCKLPM5; )


#define MCU_IO_PERIPHERAL_1(port, pin, dir) MCU_IO_PERIPHERAL_1_PREP(port, pin, dir)    //  dir is useful in some occations
#define MCU_IO_PERIPHERAL_2(port, pin, dir) MCU_IO_PERIPHERAL_2_PREP(port, pin, dir)
#define MCU_IO_PERIPHERAL_3(port, pin, dir) MCU_IO_PERIPHERAL_3_PREP(port, pin, dir)

#define MCU_IO_INPUT(port, pin, pull)       MCU_IO_INPUT_PREP(port, pin, pull)
#define MCU_IO_OUTPUT(port, pin, val)       MCU_IO_OUTPUT_PREP(port, pin, val)
#define MCU_IO_DRIVE(port, pin, drive)      MCU_IO_DRIVE_PREP(port, pin, drive)         // Must run MCU_IO_OUTPUT() first!
#define MCU_IO_SET(port, pin, val)          MCU_IO_SET_PREP(port, pin, val)
#define MCU_IO_SET_HIGH(port, pin)          MCU_IO_SET_HIGH_PREP(port, pin)
#define MCU_IO_SET_LOW(port, pin)           MCU_IO_SET_LOW_PREP(port, pin)
#define MCU_IO_TGL(port, pin)               MCU_IO_TGL_PREP(port, pin)
#define MCU_IO_GET(port, pin)               MCU_IO_GET_PREP(port, pin)                  // Only compare with 0


#define MCU_BUS_PERIPHERAL_1(port, dir)     MCU_BUS_PERIPHERAL_1_PREP(port, dir)
#define MCU_BUS_PERIPHERAL_2(port, dir)     MCU_BUS_PERIPHERAL_2_PREP(port, dir)
#define MCU_BUS_PERIPHERAL_3(port, dir)     MCU_BUS_PERIPHERAL_3_PREP(port, dir)

#define MCU_BUS_INPUT(port, pull)           MCU_BUS_INPUT_PREP(port, pull)
#define MCU_BUS_OUTPUT(port, val)           MCU_BUS_OUTPUT_PREP(port, val)
#define MCU_BUS_DRIVE(port, drive)          MCU_BUS_DRIVE_PREP(port, drive)             // Must run MCU_BUS_OUTPUT() first!
#define MCU_BUS_GET(port)                   MCU_BUS_GET_PREP(port)
#define MCU_BUS_SET(port, val)              MCU_BUS_SET_PREP(port, val)
#define MCU_BUS_TGL(port)                   MCU_BUS_TGL_PREP(port)


//  dir:
//  MCU_IO_DIR_INPUT    0
//  MCU_IO_DIR_OUTPUT   1

//  pull:
//  MCU_IO_PULLOFF      0
//  MCU_IO_TRISTATE     1
//  MCU_IO_PULLUP       2
//  MCU_IO_PULLDOWN     3

//  drive:
//  MCU_IO_DRIVE_WEAK   0               // Reduced drive strength
//  MCU_IO_DRIVE_FULL   1               // Full drive strength




//----------------------------------------------------------------------------------
// Safe polling for state change of variable set in interrupt context
//----------------------------------------------------------------------------------
#define SAFE_POLL(exit_condition, sleep_mode) \
    { istate_t key = halIntLock(); \
      while (!(exit_condition)) \
      { halMcuSetLowPowerMode((sleep_mode)); \
        key = halIntLock(); \
      } \
      halIntUnlock(key); }



//----------------------------------------------------------------------------------
//  Macros for internal use (the MCU_IO macros above need a new round in the preprocessor)
//----------------------------------------------------------------------------------
#define MCU_IO_PERIPHERAL_1_PREP(port, pin, dir)    st( P##port##SEL0 |= BIT##pin ; \
                                                    P##port##SEL1 &= ~BIT##pin ; \
                                                    if (dir) \
                                                    { P##port##DIR |= BIT##pin ; } \
                                                    else \
                                                    { P##port##DIR &= ~BIT##pin ; })
#define MCU_IO_PERIPHERAL_2_PREP(port, pin, dir)    st( P##port##SEL0 &= ~BIT##pin ; \
                                                    P##port##SEL1 |= BIT##pin ; \
                                                    if (dir) \
                                                    { P##port##DIR |= BIT##pin ; } \
                                                    else \
                                                    { P##port##DIR &= ~BIT##pin ; })
#define MCU_IO_PERIPHERAL_3_PREP(port, pin, dir)    st( P##port##SEL0 |= BIT##pin ; \
                                                    P##port##SEL1 |= BIT##pin ; \
                                                    if (dir) \
                                                    { P##port##DIR |= BIT##pin ; } \
                                                    else \
                                                    { P##port##DIR &= ~BIT##pin ; })

#define MCU_IO_INPUT_PREP(port, pin, pull)          st( P##port##SEL0 &= ~BIT##pin ; \
                                                    P##port##SEL1 &= ~BIT##pin ; \
                                                    P##port##DIR &= ~BIT##pin ;  \
                                                    if (pull == MCU_IO_PULLOFF) \
                                                    { P##port##REN &= ~BIT##pin ; } \
                                                    else if (pull == MCU_IO_PULLUP)  \
                                                        {   P##port##OUT |= BIT##pin ;  \
                                                            P##port##REN |= BIT##pin ; }  \
                                                        else if (pull == MCU_IO_PULLDOWN)  \
                                                        {   P##port##OUT &= ~BIT##pin ;  \
                                                            P##port##REN |= BIT##pin ; } ) \

#define MCU_IO_OUTPUT_PREP(port, pin, val)          st( P##port##SEL0 &= ~BIT##pin ; \
                                                    P##port##SEL1 &= ~BIT##pin ; \
                                                    MCU_IO_SET(port, pin, val); \
                                                    P##port##DIR |= BIT##pin ; )

#define MCU_IO_DRIVE_PREP(port, pin, drive)         st( if (drive == MCU_IO_DRIVE_FULL) \
                                                    { P##port##DS |= BIT##pin ; } \
                                                    else \
                                                    { P##port##DS &= ~BIT##pin ; })

#define MCU_IO_SET_HIGH_PREP(port, pin)             st( P##port##OUT |= BIT##pin ; )
#define MCU_IO_SET_LOW_PREP(port, pin)              st( P##port##OUT &= ~BIT##pin ; )
#define MCU_IO_SET_PREP(port, pin, val)             st( if (val) \
                                                    { MCU_IO_SET_HIGH(port, pin); } \
                                                    else \
                                                    { MCU_IO_SET_LOW(port, pin); })

#define MCU_IO_TGL_PREP(port, pin)                  st( P##port##OUT ^= BIT##pin ; )
#define MCU_IO_GET_PREP(port, pin)                  (P##port##IN & BIT##pin )





#define MCU_BUS_PERIPHERAL_1_PREP(port, dir)        st( P##port##SEL0 = 0xFF; \
                                                    P##port##SEL1 = 0; \
                                                    if (dir) \
                                                    { P##port##DIR = 0xFF; } \
                                                    else \
                                                    { P##port##DIR = 0; })
#define MCU_BUS_PERIPHERAL_2_PREP(port, dir)        st( P##port##SEL0 = 0; \
                                                    P##port##SEL1 = 0xFF; \
                                                    if (dir) \
                                                    { P##port##DIR = 0xFF; } \
                                                    else \
                                                    { P##port##DIR = 0; })
#define MCU_BUS_PERIPHERAL_3_PREP(port, dir)        st( P##port##SEL0 = 0xFF; \
                                                    P##port##SEL1 = 0xFF; \
                                                    if (dir) \
                                                    { P##port##DIR = 0xFF; } \
                                                    else \
                                                    { P##port##DIR = 0; })

#define MCU_BUS_INPUT_PREP(port, pull)              st( P##port##SEL0 = 0; \
                                                    P##port##SEL1 = 0; \
                                                    P##port##DIR = 0; \
                                                    if (pull == MCU_IO_PULLOFF) \
                                                    { P##port##REN = 0; } \
                                                    else if (pull == MCU_IO_PULLUP)  \
                                                        {   P##port##OUT = 0xFF;  \
                                                            P##port##REN = 0xFF; }  \
                                                        else if (pull == MCU_IO_PULLDOWN)  \
                                                        {   P##port##OUT = 0x00;  \
                                                            P##port##REN = 0xFF; } ) \

#define MCU_BUS_OUTPUT_PREP(port, val)              st( P##port##SEL0 = 0; \
                                                    P##port##SEL1 = 0; \
                                                    MCU_BUS_SET_PREP(port, val); \
                                                    P##port##DIR = 0xFF; )

#define MCU_BUS_DRIVE_PREP(port, drive)             st( if (drive == MCU_IO_DRIVE_FULL) \
                                                    { P##port##DS = 0xFF; } \
                                                    else \
                                                    { P##port##DS = 0x00; })

#define MCU_BUS_GET_PREP(port)                      (P##port##IN)
#define MCU_BUS_SET_PREP(port, val)                 st( P##port##OUT = val; )
#define MCU_BUS_TGL_PREP(port)                      st( P##port##OUT ^= 0xFF; )



//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------


/* Code Composer support */

#if defined __TI_COMPILER_VERSION__
#define NOP()  _nop()
#define _PRAGMA(x) _Pragma(#x)

#define HAL_ISR_FUNC_DECLARATION(f,v)   \
    _PRAGMA(vector=v##_VECTOR) __interrupt void f(void)
#define HAL_ISR_FUNC_PROTOTYPE(f,v)     \
    _PRAGMA(vector=v##_VECTOR) __interrupt void f(void)
//#define HAL_ISR_FUNCTION(f,v) HAL_ISR_FUNC_DECLARATION(f,v)


/*
 * Control bits in the processor status register, SR.
 */

#define __SR_GIE     (1<<3)
#define __SR_CPU_OFF (1<<4)
#define __SR_OSC_OFF (1<<5)
#define __SR_SCG0    (1<<6)
#define __SR_SCG1    (1<<7)


/*
 * Functions for controlling the processor operation modes.
 */

//#define __low_power_mode_0() (__bis_SR_register(  __SR_GIE      \
                                                | __SR_CPU_OFF))

//#define __low_power_mode_1() (__bis_SR_register(  __SR_GIE      \
                                                | __SR_CPU_OFF  \
                                                | __SR_SCG0))

//#define __low_power_mode_2() (__bis_SR_register(  __SR_GIE      \
                                                | __SR_CPU_OFF  \
                                                | __SR_SCG1))

//#define __low_power_mode_3()                    \
  (_bis_SR_register(  __SR_GIE                  \
                     | __SR_CPU_OFF             \
                     | __SR_SCG0                \
                     | __SR_SCG1))

//#define __low_power_mode_4()                    \
  (_bis_SR_register(  __SR_GIE                  \
                     | __SR_CPU_OFF             \
                     | __SR_SCG0                \
                     | __SR_SCG1                \
                     | __SR_OSC_OFF))

//#define __low_power_mode_off_on_exit()          \
  (_bic_SR_register_on_exit(  __SR_CPU_OFF      \
                             | __SR_SCG0        \
                             | __SR_SCG1        \
                             | __SR_OSC_OFF))
#endif

#endif /* INCLUDE_MACROS_H_ */
