/*
 * os_task.c
 *
 *  Created on: 2018Äê3ÔÂ13ÈÕ
 *      Author: carpela
 */

#include <task.h>
#include <mm.h>
#include <intrinsics.h>

int numTasks = 0;

task_struct tasks[MAX_TASKS];

task_struct *taskLst, *taskFreeLst;

task_struct* current;

task_struct* get_available_tcb(void)
{
    if (taskFreeLst == NULL)
        return NULL;
    task_struct* tcb = taskFreeLst;
    taskFreeLst = taskFreeLst->next;
    if (taskFreeLst != NULL)
        taskFreeLst->prev = NULL;
    return tcb;
}

void put_back_tcb(task_struct* tcb)
{
    tcb->prev->next = tcb->next;
    tcb->next->prev = tcb->prev;
    tcb->next = taskFreeLst;
    if (taskFreeLst != NULL)
        taskFreeLst->prev = tcb;
    tcb->prev = NULL;
    taskFreeLst = tcb;
}

int task_create(task task, void * param)
{
    int i;
    task_stack *stack;
    task_struct* taskTCB = get_available_tcb();
    if (taskTCB == NULL)
        return -1;
    taskTCB->state = TASK_UNINTERRUPTIBLE;

    stack = (task_stack*) mem_alloc_no_release(TASK_STACK_SIZE*2);
    for(i=0;i<28;i++)
    {
        stack[TASK_STACK_SIZE-i] = 0;
    }
    stack[TASK_STACK_SIZE-1] = (unsigned long) task_exit >> 16;
    stack[TASK_STACK_SIZE-2] = (unsigned long) task_exit;
    stack[TASK_STACK_SIZE-3] = (unsigned long) task;
    stack[TASK_STACK_SIZE-4] = ((unsigned long) task & 0xf0000) >> 4 | 8;
    stack[TASK_STACK_SIZE-11] = (unsigned long) param >> 16;
    stack[TASK_STACK_SIZE-12] = (unsigned long) param;

    taskTCB->next = taskLst;
    taskTCB->prev = NULL;
    if (taskLst != NULL)
        taskLst->prev = taskTCB;

    taskTCB->sp = (unsigned long) stack + TASK_STACK_SIZE*sizeof(task_stack) - 15*4;
    taskLst = taskTCB;

    taskTCB->priority = 15;
    taskTCB->counter = 15;
    numTasks++;
    taskTCB->state = TASK_RUNNING;
    taskTCB->signal = 0;
    taskTCB->blocked = 0;
    taskTCB->sig_handler = NULL;
    return (taskTCB-tasks);
}

void task_destory(task_struct* tcb)
{
    _disable_interrupts();
    numTasks--;
    tcb->state = TASK_ZOMBIE;
    put_back_tcb(tcb);
    if (tcb == current)
        schedule();
    _enable_interrupts();
}

void task_exit(void)
{
    _disable_interrupts();
    numTasks--;
    current->state = TASK_ZOMBIE;
    put_back_tcb(current);
    schedule();
}

void task_pause(task_struct* tcb)
{
    _disable_interrupts();
    tcb->state = TASK_INTERRUPTIBLE;
    if (tcb == current)
        schedule();
    _enable_interrupts();
}

void task_resume(task_struct* tcb)
{
    if (tcb == current)
        return;
    _disable_interrupts();
    if (tcb->state == TASK_INTERRUPTIBLE)
        tcb->state = TASK_RUNNING;
    schedule();
    _enable_interrupts();
}

void task_sleep(task_struct* tcb)
{
    _disable_interrupts();
    tcb->state = TASK_UNINTERRUPTIBLE;
    if (tcb == current)
        schedule();
    _enable_interrupts();
}

void task_init(void)
{
    int i;
    taskLst = NULL;
    current = NULL;
    taskFreeLst = &tasks[0];

    for (i = 0; i < MAX_TASKS-1; i++)
        tasks[i].next = &tasks[i+1];
    tasks[MAX_TASKS-1].next = NULL;

    tasks[0].prev = NULL;
    for (i = 1; i < MAX_TASKS; i++)
        tasks[i].prev = &tasks[i-1];
}
