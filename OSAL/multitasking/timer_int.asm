
;				Layout of ISR STACK
;	---------------------------------------------
;	|			PC(lower 16 bits)				|
;	---------------------------------------------
;	|	PC(upper 4 bits) & SR(lower 12 bits)	|
;	---------------------------------------------
;	|											|
;	|	 			R15~R3						|
;	|											|
;	---------------------------------------------		<---- SP (in struct task)
;
;	Notes:	when returns from ISR, stack frame can be of current task (no schedule) or next task (schedule)

	.cdecls 	C,LIST,"task.h"
	.cdecls 	C,LIST,"msp430.h"
	.global		jiffies,current
	.global		schedule,task_sched_next
	.def		TIMER_INT_ASM
	.sect		".text:_isr"			; see ld scripts
TIMER_INT_ASM:
	PUSHM.A		#13,R15

	INC.W		&jiffies				; jiffies is 32-bit unsigned long
	ADC.W		&jiffies+2

	MOVA		&current,R15
	DEC.W		TASK_COUNTER(R15)
	JNE			int_ret

	MOVA		SP,R10					; R10 as stack frame pointer
	CALLA		#task_sched_next
	CMPX.A		&current,R12
	JEQ			int_ret

	MOVA		R10,SP
	MOVA		&current,R15
	MOVA		SP,TASK_SP(R15)			; switch stack
	MOVA		TASK_SP(R12),SP
	MOVA		R12,&current

	MOVA		TASK_SIG_FUNC(R12),R13
	CMPA		#0,R13
	JEQ			int_ret
	MOVA		R12,R15
	MOV			TASK_SIGNAL(R15),R12
	XOR			TASK_BLOCKED(R15),R12
	CALLA		R13
int_ret:
	POPM.A		#13,R15
	RETI

	.sect	TIMER0_A0_VECTOR					; Timer0_A0_VECTOR
	.short	TIMER_INT_ASM
