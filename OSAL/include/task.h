/*
 * os_task.h
 *
 *  Created on: 2018Äê3ÔÂ13ÈÕ
 *      Author: carpela
 */

#ifndef OSAL_OS_TASK_H_
#define OSAL_OS_TASK_H_

#include <configs.h>

typedef int task_stack;
typedef void (*task)(void * param);

typedef void (*sig_func)(unsigned int signal);
/*********************************
 * Definition for OSAL TCB
 *
 *********************************/
typedef struct task_struct
{
    /* Registers */
   long sp;

    /* fields for scheduler */
    short state;
    short priority;
    short counter;

    /* Signal */
    unsigned int signal;
    unsigned int blocked;
    sig_func sig_handler;

    /* TCB link */
    struct task_struct* next;
    struct task_struct* prev;
} task_struct;

#ifdef  __ASM_HEADER__
#define TASK_SP 0
#define TASK_STATE  4
#define TASK_COUNTER    8
#define TASK_SIGNAL 10
#define TASK_BLOCKED    12
#define TASK_SIG_FUNC   14
#else
#define OFFSETOF(TYPE,MEMBER)   ((int) &((TYPE*)0)->MEMBER)
#define TASK_SP OFFSETOF(task_struct,sp)
#define TASK_STATE  OFFSETOF(task_struct,state)
#define TASK_COUNTER    OFFSETOF(task_struct,counter)
#define TASK_SIGNAL OFFSETOF(task_struct,signal)
#define TASK_BLOCKED    OFFSETOF(task_struct,blocked)
#define TASK_SIG_FUNC   OFFSETOF(task_struct,sig_handler)
#endif

extern task_struct tasks[MAX_TASKS];

extern task_struct *taskLst, *taskFreeLst;

extern task_struct* current;

extern int numTasks;

extern void task_init(void);
extern void osal_start(void);
extern int task_create(task task, void * param);
extern void schedule(void);

#define TASK_RUNNING        0
#define TASK_INTERRUPTIBLE  1
#define TASK_UNINTERRUPTIBLE    2
#define TASK_ZOMBIE     3
#define TASK_STOPPED        4

extern unsigned long start_time;
extern unsigned long jiffies;
extern unsigned short nested;

extern void switch_to_task(task_struct * tcb);
extern void switch_to_task_int(task_struct * tcb);
extern void sched_start(void);
extern void ret_from_switch(void);
extern void task_exit(void);

#endif /* OSAL_OS_TASK_H_ */
