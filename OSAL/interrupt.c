/*
 * interrupt.c
 *
 *  Created on: 2018��4��27��
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
