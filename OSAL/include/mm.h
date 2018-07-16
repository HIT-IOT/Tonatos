/*
 * mm.h
 *
 *  Created on: 2018Äê5ÔÂ26ÈÕ
 *      Author: carpela
 */

#ifndef OSAL_INCLUDE_MM_H_
#define OSAL_INCLUDE_MM_H_

void * mem_alloc_no_release(unsigned long size);


typedef struct mcb {
    long addr;
    long freeLst;
    int nBlks;
    int nFreeBlks;
    int blockSize;
} mcb;
#endif /* OSAL_INCLUDE_MM_H_ */
