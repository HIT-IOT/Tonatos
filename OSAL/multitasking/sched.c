/*
 * sched.c
 *
 *  Created on: 2018Äê5ÔÂ17ÈÕ
 *      Author: carpela
 */
#include <task.h>
#include <msp430.h>
#include <intrinsics.h>

unsigned long jiffies = 0;
unsigned long start_time = 0;

task_struct* task_sched_next(void)
{
    task_struct* selected = NULL;
    task_struct* ts = taskLst;
    int c;

    while(1)
    {
        c = -1;
        selected = NULL;
        for (ts=taskLst; ts!=NULL; ts=ts->next) {
            if (ts->state == TASK_RUNNING && ts->counter > c)
                c = ts->counter, selected = ts;
        }
        if (c) break;
        for (ts=taskLst; ts!=NULL; ts=ts->next)
            ts->counter = ts->counter + ts->priority;
    }
    return selected;
}

/*
 * This function shouldn't be called directly from user application.
 */
void schedule(void)
{
    task_struct* next = task_sched_next();
    if (next == current)
        return;
    switch_to_task(next);
}

