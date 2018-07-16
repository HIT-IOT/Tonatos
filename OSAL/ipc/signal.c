#include <task.h>

void send_signal(task_struct *tcb, unsigned int sig)
{
    tcb->signal |= sig;
}

void set_block(task_struct *tcb, unsigned int sig)
{
    tcb->blocked |= sig;
}

void install_sig_handler(task_struct *tcb, sig_func func)
{
    tcb->sig_handler = func;
}
