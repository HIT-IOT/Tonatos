/*
 * sem.h
 *
 *  Created on: 2018Äê5ÔÂ23ÈÕ
 *      Author: carpela
 */

#ifndef OSAL_INCLUDE_SEM_H_
#define OSAL_INCLUDE_SEM_H_

#define QUE_LEN 16
#define SEM_FAILED  (void*) 0
struct semaphore_queue
{
    int front;
    int rear;
    struct task_struct *wait_tasks[QUE_LEN];
};
typedef struct semaphore_queue sem_queue;

struct semaphore_t
{
    int value;
    int occupied;
    char name[16];
    struct semaphore_queue wait_queue;
};
typedef struct semaphore_t sem_t;

#endif /* OSAL_INCLUDE_SEM_H_ */
