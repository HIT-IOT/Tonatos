/*
 * types.h
 *
 *  Created on: 2018Äê5ÔÂ30ÈÕ
 *      Author: carpela
 */

#ifndef INCLUDE_TYPES_H_
#define INCLUDE_TYPES_H_

#include <stdint.h>
/* Definitions for (u)int8/16/32 etc */
typedef signed   char   int8;
typedef unsigned char   uint8;

typedef signed   short  int16;
typedef unsigned short  uint16;

typedef signed   long   int32;
typedef unsigned long   uint32;
typedef unsigned short  istate_t;

#include <stdbool.h>
#define TRUE    true
#define FALSE   false
typedef void (*ISR_FUNC_PTR)(void);
typedef void (*VFPTR)(void);


#define _PRAGMA(x) _Pragma(#x)


#ifndef CODE
#define CODE
#endif
#ifndef XDATA
#define XDATA
#endif
#define FAR
#define NOP()  asm("NOP")

#define HAL_ISR_FUNC_DECLARATION(f,v)   \
    _PRAGMA(vector=v##_VECTOR) __interrupt void f(void)
#define HAL_ISR_FUNC_PROTOTYPE(f,v)     \
    _PRAGMA(vector=v##_VECTOR) __interrupt void f(void)

#define HAL_ISR_FUNCTION(f,v)           \
    HAL_ISR_FUNC_DECLARATION(f,v)

#endif /* INCLUDE_TYPES_H_ */
