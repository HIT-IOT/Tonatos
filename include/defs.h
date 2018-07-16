/*
 * general.h
 *
 *  Created on: 2018Äê5ÔÂ29ÈÕ
 *      Author: carpela
 */

#ifndef INCLUDE_DEFS_H_
#define INCLUDE_DEFS_H_

#define NULL (void*)0
#ifndef BV
#define BV(n)      (1 << (n))
#endif

#ifndef BM
#define BM(n)      (1 << (n))
#endif

#ifndef BF
#define BF(x,b,s)  (((x) & (b)) >> (s))
#endif

#ifndef MIN
#define MIN(n,m)   (((n) < (m)) ? (n) : (m))
#endif

#ifndef MAX
#define MAX(n,m)   (((n) < (m)) ? (m) : (n))
#endif

#ifndef ABS
#define ABS(n)     (((n) < 0) ? -(n) : (n))
#endif


/* uint32 processing */
#define BREAK_UINT32( var, ByteNum ) \
    (uint8)((uint32)(((var) >>((ByteNum) * 8)) & 0x00FF))

#define BUILD_UINT32(Byte0, Byte1, Byte2, Byte3) \
    ((uint32)((uint32)((Byte0) & 0x00FF) \
        + ((uint32)((Byte1) & 0x00FF) << 8) \
            + ((uint32)((Byte2) & 0x00FF) << 16) \
                + ((uint32)((Byte3) & 0x00FF) << 24)))

#define HI_UINT32(a) ((uint16) (((uint32)(a)) >> 16))
#define LO_UINT32(a) ((uint16) ((uint32)(a)))


/* uint16 processing */
#define BUILD_UINT16(loByte, hiByte) \
    ((uint16)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

#define HI_UINT16(a) (((uint16)(a) >> 8) & 0xFF)
#define LO_UINT16(a) ((uint16)(a) & 0xFF)


/* uint16 processing */
#define BUILD_UINT8(hiByte, loByte) \
    ((uint8)(((loByte) & 0x0F) + (((hiByte) & 0x0F) << 4)))

#define HI_UINT8(a) (((uint8)(a) >> 4) & 0x0F)
#define LO_UINT8(a) ((uint8)(a) & 0x0F)

/***********************************************************************************
 * Host to network byte order macros
 */
#ifdef BIG_ENDIAN
#define UINT16_HTON(x)  st( utilReverseBuf((uint8*)&x, sizeof(uint16)); )
#define UINT16_NTOH(x)  st( utilReverseBuf((uint8*)&x, sizeof(uint16)); )

#define UINT32_HTON(x)  st( utilReverseBuf((uint8*)&x, sizeof(uint32)); )
#define UINT32_NTOH(x)  st( utilReverseBuf((uint8*)&x, sizeof(uint32)); )
#else
#define UINT16_HTON(x)
#define UINT16_NTOH(x)

#define UINT32_HTON(x)
#define UINT32_NTOH(x)
#endif
/*
*  This macro is for use by other macros to form a fully valid C statement.
*  Without this, the if/else conditionals could show unexpected behavior.
*
*  For example, use...
*    #define SET_REGS()  st( ioreg1 = 0; ioreg2 = 0; )
*  instead of ...
*    #define SET_REGS()  { ioreg1 = 0; ioreg2 = 0; }
*  or
*    #define  SET_REGS()    ioreg1 = 0; ioreg2 = 0;
*  The last macro would not behave as expected in the if/else construct.
*  The second to last macro will cause a compiler error in certain uses
*  of if/else construct
*
*  It is not necessary, or recommended, to use this macro where there is
*  already a valid C statement.  For example, the following is redundant...
*    #define CALL_FUNC()   st(  func();  )
*  This should simply be...
*    #define CALL_FUNC()   func()
*
* (The while condition below evaluates false without generating a
*  constant-controlling-loop type of warning on most compilers.)
*/
#define __IEA__
//#define __MSPEXP430FR5969_Launch_Pad__
#define st(x)      do { x } while (__LINE__ == -1)


#endif /* INCLUDE_DEFS_H_ */
