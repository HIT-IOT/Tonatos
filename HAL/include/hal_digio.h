/******************************************************************************
  Filename:     hal_digio.h

  Description:  HAL digital IO functionality

******************************************************************************/

#ifndef HAL_DIGIO_H
#define HAL_DIGIO_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * INCLUDES
 */
#include <types.h>

/******************************************************************************
 * CONSTANTS AND DEFINES
 */
enum {
    HAL_DIGIO_INPUT,
    HAL_DIGIO_OUTPUT
};

enum {
    HAL_DIGIO_INT_FALLING_EDGE,
    HAL_DIGIO_INT_RISING_EDGE
};

#define HAL_DIGIO_OK       0
#define HAL_DIGIO_ERROR  (~0)

/******************************************************************************
 * TYPEDEFS
 */

typedef struct {
    uint8 port;     // port number
    uint8 pin;      // pin number
    uint8 pin_bm;   // pin bitmask
    uint8 dir;      // direction (input or output)
    uint8 initval;  // initial value
} digioConfig;


extern bool IO_WAKE_INT;
/******************************************************************************
 * GLOBAL FUNCTIONS
 */
uint8 halDigioConfig(const digioConfig* p);
uint8 halDigioClear(const digioConfig* p);
uint8 halDigioToggle(const digioConfig* p);
uint8 halDigioSet(const digioConfig* p);
uint8 halDigioGet(const digioConfig* p);

uint8 halDigioIntConnect(const digioConfig *p, ISR_FUNC_PTR func);
uint8 halDigioIntEnable(const digioConfig *p);
uint8 halDigioIntDisable(const digioConfig *p);
uint8 halDigioIntClear(const digioConfig *p);
uint8 halDigioIntSetEdge(const digioConfig *p, uint8 edge);


#ifdef  __cplusplus
}
#endif


#endif
