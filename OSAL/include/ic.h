/*
 * ic.h
 *
 *  Created on: 2018��6��3��
 *      Author: carpela
 */

#ifndef OSAL_INCLUDE_IC_H_
#define OSAL_INCLUDE_IC_H_

void store_context(void);
int exist_context(void);
void restore_context(void);
void init_context(void);

#endif /* OSAL_INCLUDE_IC_H_ */
