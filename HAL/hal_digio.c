/***********************************************************************************
  Filename:     hal_digio.c

  Description:  HAL digital IO functionality

    配置标准 IO
    Fit to FR5x6x
    暂不支持上拉/下拉/强弱驱动
    支持端口：10 中断端口：4，中断查询使用PxIV
    张扬  2016.12.9

***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/
#include <types.h>
#include <defs.h>
#include <msp430.h>
#include <hal_digio.h>
#include <hal_int.h>

typedef unsigned short istate_t;
/***********************************************************************************
* LOCAL VARIABLES
*/
static ISR_FUNC_PTR port1_isr_tbl[8] = {0};
static ISR_FUNC_PTR port2_isr_tbl[8] = {0};
static ISR_FUNC_PTR port3_isr_tbl[8] = {0};
static ISR_FUNC_PTR port4_isr_tbl[8] = {0};


bool    IO_WAKE_INT = FALSE;

/***********************************************************************************
* GLOBAL FUNCTIONS
*/

/***********************************************************************************
* @fn      halDigioConfig
*
* @brief   Configure the pin as specified by p.
*
* @param   digioConfig* p - pointer to configuration structure for IO pin
*
* @return  uint8 - HAL_DIGIO_ERROR or HAL_DIGIO_OK
*/
uint8 halDigioConfig(const digioConfig* p)
{
    register volatile uint8* dir;
    register volatile uint8* out;
    register const uint8 bitmask = p->pin_bm;

    // Sanity check
    if ((bitmask == 0) || (bitmask != (uint8)BV(p->pin)))
    {
        return(HAL_DIGIO_ERROR);
    }

    switch(p->port)
    {
    case 1: P1SEL0 &= ~bitmask; P1SEL1 &= ~bitmask; out = &P1OUT; dir = &P1DIR; break;
    case 2: P2SEL0 &= ~bitmask; P2SEL1 &= ~bitmask; out = &P2OUT; dir = &P2DIR; break;
    case 3: P3SEL0 &= ~bitmask; P3SEL1 &= ~bitmask; out = &P3OUT; dir = &P3DIR; break;
    case 4: P4SEL0 &= ~bitmask; P4SEL1 &= ~bitmask; out = &P4OUT; dir = &P4DIR; break;
    case 5: P5SEL0 &= ~bitmask; P5SEL1 &= ~bitmask; out = &P5OUT; dir = &P5DIR; break;
    case 6: P6SEL0 &= ~bitmask; P6SEL1 &= ~bitmask; out = &P6OUT; dir = &P6DIR; break;
    case 7: P7SEL0 &= ~bitmask; P7SEL1 &= ~bitmask; out = &P7OUT; dir = &P7DIR; break;
    case 9: P9SEL0 &= ~bitmask; P9SEL1 &= ~bitmask; out = &P9OUT; dir = &P9DIR; break;
    default: return(HAL_DIGIO_ERROR);
    }

    if (p->dir == HAL_DIGIO_OUTPUT)
    {
        if (p->initval == 1)
        {
            *out |= bitmask;
        }
        else
        {
            *out &= ~bitmask;
        }
        *dir |= bitmask;
    }
    else // input
    {
        *out &= ~bitmask;
        *dir &= ~bitmask;
    }
    return(HAL_DIGIO_OK);
}


/***********************************************************************************
* @fn      halDigioSet
*
* @brief   Set output pin
*
* @param   digioConfig* p - pointer to configuration structure for IO pin
*
* @return  uint8 - HAL_DIGIO_ERROR or HAL_DIGIO_OK
*/
uint8 halDigioSet(const digioConfig* p)
{
    if (p->dir == HAL_DIGIO_OUTPUT)
    {
        switch (p->port)
        {
        case 1: P1OUT |= p->pin_bm; break;
        case 2: P2OUT |= p->pin_bm; break;
        case 3: P3OUT |= p->pin_bm; break;
        case 4: P4OUT |= p->pin_bm; break;
        case 5: P5OUT |= p->pin_bm; break;
        case 6: P6OUT |= p->pin_bm; break;
        case 7: P7OUT |= p->pin_bm; break;
        case 9: P9OUT |= p->pin_bm; break;
        default: return(HAL_DIGIO_ERROR);
        }
        return(HAL_DIGIO_OK);
    }
    return(HAL_DIGIO_ERROR);

}

/***********************************************************************************
* @fn      halDigioClear
*
* @brief   Clear output pin
*
* @param   digioConfig* p - pointer to configuration structure for IO pin
*
* @return  uint8 - HAL_DIGIO_ERROR or HAL_DIGIO_OK
*/
uint8 halDigioClear(const digioConfig* p)
{
    if (p->dir == HAL_DIGIO_OUTPUT)
    {
        switch (p->port)
        {
        case 1: P1OUT &= ~p->pin_bm; break;
        case 2: P2OUT &= ~p->pin_bm; break;
        case 3: P3OUT &= ~p->pin_bm; break;
        case 4: P4OUT &= ~p->pin_bm; break;
        case 5: P5OUT &= ~p->pin_bm; break;
        case 6: P6OUT &= ~p->pin_bm; break;
        case 7: P7OUT &= ~p->pin_bm; break;
        case 9: P9OUT &= ~p->pin_bm; break;
        default: return(HAL_DIGIO_ERROR);
        }
        return(HAL_DIGIO_OK);
    }
    return(HAL_DIGIO_ERROR);
}


/***********************************************************************************
* @fn      halDigioToggle
*
* @brief   Toggle output pin
*
* @param   digioConfig* p - pointer to configuration structure for IO pin
*
* @return  uint8 - HAL_DIGIO_ERROR or HAL_DIGIO_OK
*/
uint8 halDigioToggle(const digioConfig* p)
{
    if (p->dir == HAL_DIGIO_OUTPUT)
    {
        switch (p->port)
        {
        case 1: P1OUT ^= p->pin_bm; break;
        case 2: P2OUT ^= p->pin_bm; break;
        case 3: P3OUT ^= p->pin_bm; break;
        case 4: P4OUT ^= p->pin_bm; break;
        case 5: P5OUT ^= p->pin_bm; break;
        case 6: P6OUT ^= p->pin_bm; break;
        case 7: P7OUT ^= p->pin_bm; break;
        case 9: P9OUT ^= p->pin_bm; break;
        default: return(HAL_DIGIO_ERROR);
        }
        return(HAL_DIGIO_OK);
    }
    return(HAL_DIGIO_ERROR);
}


/***********************************************************************************
* @fn      halDigioGet
*
* @brief   Get value on input pin
*
* @param   digioConfig* p - pointer to configuration structure for IO pin
*
* @return  uint8 - HAL_DIGIO_ERROR or HAL_DIGIO_OK
*/
uint8 halDigioGet(const digioConfig* p)
{
    if (p->dir == HAL_DIGIO_INPUT)
    {
        switch (p->port)
        {
        case 1: return (P1IN & p->pin_bm ? 1 : 0);
        case 2: return (P2IN & p->pin_bm ? 1 : 0);
        case 3: return (P3IN & p->pin_bm ? 1 : 0);
        case 4: return (P4IN & p->pin_bm ? 1 : 0);
        case 5: return (P5IN & p->pin_bm ? 1 : 0);
        case 6: return (P6IN & p->pin_bm ? 1 : 0);
        case 7: return (P7IN & p->pin_bm ? 1 : 0);
        case 9: return (P9IN & p->pin_bm ? 1 : 0);
        default: break;
        }
    }
    return(HAL_DIGIO_ERROR);
}


/***********************************************************************************
* @fn      halDigioIntConnect
*
* @brief   Connect function to IO interrupt
*
* @param   digioConfig* p - pointer to configuration structure for IO pin
*          ISR_FUNC_PTR func - pointer to function
*
* @return  uint8 - HAL_DIGIO_ERROR or HAL_DIGIO_OK
*/
uint8 halDigioIntConnect(const digioConfig *p, ISR_FUNC_PTR func)
{
    istate_t key;
    HAL_INT_LOCK(key);
    switch (p->port)
    {
    case 1: port1_isr_tbl[p->pin] = func; break;
    case 2: port2_isr_tbl[p->pin] = func; break;
    case 3: port3_isr_tbl[p->pin] = func; break;
    case 4: port4_isr_tbl[p->pin] = func; break;
    default: HAL_INT_UNLOCK(key); return(HAL_DIGIO_ERROR);
    }
    halDigioIntClear(p);
    HAL_INT_UNLOCK(key);
    return(HAL_DIGIO_OK);
}


/***********************************************************************************
* @fn      halDigioIntEnable
*
* @brief   Enable interrupt on IO pin
*
* @param   digioConfig* p - pointer to configuration structure for IO pin
*
* @return  uint8 - HAL_DIGIO_ERROR or HAL_DIGIO_OK
*/
uint8 halDigioIntEnable(const digioConfig *p)
{
    switch (p->port)
    {
    case 1: P1IE |= p->pin_bm; break;
    case 2: P2IE |= p->pin_bm; break;
    case 3: P3IE |= p->pin_bm; break;
    case 4: P4IE |= p->pin_bm; break;
    default: return(HAL_DIGIO_ERROR);
    }
    return(HAL_DIGIO_OK);
}


/***********************************************************************************
* @fn      halDigioIntDisable
*
* @brief   Disable interrupt on IO pin
*
* @param   digioConfig* p - pointer to configuration structure for IO pin
*
* @return  uint8 - HAL_DIGIO_ERROR or HAL_DIGIO_OK
*/
uint8 halDigioIntDisable(const digioConfig *p)
{
    switch (p->port)
    {
    case 1: P1IE &= ~p->pin_bm; break;
    case 2: P2IE &= ~p->pin_bm; break;
    case 3: P3IE &= ~p->pin_bm; break;
    case 4: P4IE &= ~p->pin_bm; break;
    default: return(HAL_DIGIO_ERROR);
    }
    return(HAL_DIGIO_OK);
}


/***********************************************************************************
* @fn      halDigioIntDisable
*
* @brief   Clear interrupt flag
*
* @param   digioConfig* p - pointer to configuration structure for IO pin
*
* @return  uint8 - HAL_DIGIO_ERROR or HAL_DIGIO_OK
*/
uint8 halDigioIntClear(const digioConfig *p)
{
    switch (p->port)
    {
    case 1: P1IFG &= ~p->pin_bm; break;
    case 2: P2IFG &= ~p->pin_bm; break;
    case 3: P3IFG &= ~p->pin_bm; break;
    case 4: P4IFG &= ~p->pin_bm; break;
    default: return(HAL_DIGIO_ERROR);
    }
    return(HAL_DIGIO_OK);
}


/***********************************************************************************
* @fn      halDigioIntSetEdge
*
* @brief   Set edge for IO interrupt
*
* @param   digioConfig* p - pointer to configuration structure for IO pin
*          edge - HAL_DIGIO_INT_FALLING_EDGE or HAL_DIGIO_INT_RISING_EDGE
*
* @return  uint8 - HAL_DIGIO_ERROR or HAL_DIGIO_OK
*/
uint8 halDigioIntSetEdge(const digioConfig *p, uint8 edge)
{
    switch(edge)
    {
    case HAL_DIGIO_INT_FALLING_EDGE:
        switch(p->port)
        {
        case 1: P1IES |= p->pin_bm; break;
        case 2: P2IES |= p->pin_bm; break;
        case 3: P3IES |= p->pin_bm; break;
        case 4: P4IES |= p->pin_bm; break;
        default: return(HAL_DIGIO_ERROR);
        }
        break;

    case HAL_DIGIO_INT_RISING_EDGE:
        switch(p->port)
        {
        case 1: P1IES &= ~p->pin_bm; break;
        case 2: P2IES &= ~p->pin_bm; break;
        case 3: P3IES &= ~p->pin_bm; break;
        case 4: P4IES &= ~p->pin_bm; break;
        default: return(HAL_DIGIO_ERROR);
        }
        break;

    default:
        return(HAL_DIGIO_ERROR);
    }
    return(HAL_DIGIO_OK);
}


/***********************************************************************************
* @fn      port1_ISR
*
* @brief   ISR framework for P1 digio interrupt
*/
HAL_ISR_FUNCTION(port1_ISR,PORT1)
{
    register uint8 i = P1IV >>1;
    if ((i!=0)&&(port1_isr_tbl[i-1] != 0))  (*port1_isr_tbl[i-1])();
    if (IO_WAKE_INT)    __low_power_mode_off_on_exit();                     // 退出DMA并唤醒CPU
    IO_WAKE_INT = FALSE;
}

/***********************************************************************************
* @fn      port2_ISR
*
* @brief   ISR framework for P2 digio interrupt
*/
HAL_ISR_FUNCTION(port2_ISR,PORT2)
{
    register uint8 i = P2IV >>1;
    if ((i!=0)&&(port2_isr_tbl[i-1] != 0))  (*port2_isr_tbl[i-1])();
    if (IO_WAKE_INT)    __low_power_mode_off_on_exit();                     // 退出DMA并唤醒CPU
    IO_WAKE_INT = FALSE;
}

/***********************************************************************************
* @fn      port3_ISR
*
* @brief   ISR framework for P3 digio interrupt
*/
HAL_ISR_FUNCTION(port3_ISR,PORT3)
{
    register uint8 i = P3IV >>1;
    if ((i!=0)&&(port3_isr_tbl[i-1] != 0))  (*port3_isr_tbl[i-1])();
    if (IO_WAKE_INT)    __low_power_mode_off_on_exit();                     // 退出DMA并唤醒CPU
    IO_WAKE_INT = FALSE;
}

/***********************************************************************************
* @fn      port4_ISR
*
* @brief   ISR framework for P4 digio interrupt
*/
HAL_ISR_FUNCTION(port4_ISR,PORT4)
{
    register uint8 i = P4IV >>1;
    if ((i!=0)&&(port4_isr_tbl[i-1] != 0))  (*port4_isr_tbl[i-1])();
    if (IO_WAKE_INT)    __low_power_mode_off_on_exit();                     // 退出DMA并唤醒CPU
    IO_WAKE_INT = FALSE;
}
