/*
 * init.c
 *
 *  Created on: 2018��4��26��
 *      Author: carpela
 */
#include <task.h>

void osal_init() {

    /* RTC timer */
    start_time = 0;
    task_init();
}

void osal_start() {
    sched_start();
    // should never be here
    while(1) {
        schedule();
    }
}
