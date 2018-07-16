
	.cdecls 	C,LIST,"task.h"
	.global switch_to_task,ret_from_switch,sched_start
	.global current,taskLst

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
switch_to_task:
	PUSHA		R13
	MOV.W		6(SP),R13
	MOV.W		4(SP),6(SP)
	RRC.W		R13
	RRCM.W		#4,R13									; RRCM only supports up to 4 bits
	BIS.W		SR,R13
	MOV.W		R13,4(SP)
	POPA		R13

	PUSHM.A		#13,R15
	MOVA		&current,R15
	MOVA		SP,TASK_SP(R15)

	MOVA		TASK_SP(R12),SP
	MOVA		R12,&current
	POPM.A		#13,R15
ret_from_switch:
	RETI

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
sched_start:
	MOVA		&taskLst,R4
	CMPA		#0,R4
	JEQ			skip									; No task
	MOVA		TASK_SP(R4),SP
	MOVA		R4,&current
	POPM.A		#13,R15
	BIS			SR,TASK_SP(SP)
skip:
	RETI
