/*
 * interrupt.c
 *
 *  Created on: 2018Äê4ÔÂ27ÈÕ
 *      Author: carpela
 */
#include <intrinsics.h>
#include <msp430.h>
#include <task.h>

void eint()
{
    _enable_interrupts();
}

void dint()
{
    _disable_interrupts();
}
