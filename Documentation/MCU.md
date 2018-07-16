/*
	ϵͳӲ����ʼ�� v1.0
	2014.12.16

	����ʱ�䣺��190mS


ϵͳӲ����Դ���ã�	430FR6979IPN

	ʱ��ϵͳ��
ACLK:	32K OSC		/1
MCLK:	4MHz DCO	/1		��ѡ���þ���
SMCLK:	MCLK		/1

	LCD��
		��0-��7������δʹ��
		ACLK��64Hz��3.02V

	USCI:
		A0��	��Ƶͨ��		��SPI��
		B0��	IO��LCDռ��
		A1��	�����			��SPI��
		B1��	RTC��EEPROM		(IIC)

	��ʱ����
		TA0.0:	��ѹ�ã�		ACLK/1��	���ڿɱ䣬	���蹤�����жϹ�
		TA0.1:	��ѹ�����ʱ��
		TA0.2:

		TA1.0:	C2��ŵ���ƣ�	ACLK/8��	���ڿɱ䣬	��Ϲ������жϹ�
		TA1.1:	C2�ŵ縺ѹ��
		TA1.2:	C2���PFM���

		TA2.0:	ͨ��DMA������	SMCLK/4��	10ѭ����	��Ϲ������жϹ�
		TA2.1:

		TA3.0��	ϵͳRTC��		ACLK/8��	4096ѭ����	�����������жϿ�		(����ʱ���ڵȴ���������)
		TA3.1:	��ʱ�����߻��ѣ���ʱʹ�ã�
		TA3.2:
		TA3.3:
		TA3.4:

		TB0.0:
		TB0.1:
		TB0.2:
		TB0.3:
		TB0.4:
		TB0.5:
		TB0.6:

	DMA:
		ͨ��0:	��Ƶͨ�ţ����Σ�����Դ DMA0TSEL__TA2CCR0
		ͨ��1:
		ͨ��2:	�����ʾ�������Σ�����Դ DMA2TSEL__UCA1TXIFG

	AD:



	IO������±�

*/


//	Ӳ����ײ���
#include <hal_board.h>



sysStat sys_status;							// ϵͳ״̬
uint32 HAL_CPU_CLOCK_Hz;					// ��Ƶ. ��λ��Hz
uint16 HAL_CPU_CLOCK_KHz;					// ��Ƶ. ��λ��KHz. up to 65M
uint16 HAL_CPU_CLOCK_DELAY;					// ��ʱ����ר�ã��������

bool sync_flag;								// ͬ����־


/*	IEA v2.3 I/O List:					Operation mode						Low power mode

	LCD_C:
			S0-S3		P1	4-7				Sz - 1								OUTPUT	LOW
			S4-S7		P4	0/1/6/7			Sz - 1								OUTPUT	LOW
			COM			P6	3-6				SEL-3								OUTPUT	LOW

	DIAG_LCD:
			STE			P3.7				SEL-1								OUTPUT	LOW
			SCL			P3.6				SEL-1								OUTPUT	LOW
			MISO		P3.5				SEL-1								INPUT
			MOSI		P3.4				SEL-1								OUTPUT	LOW

	COMM_SX1211:
			CS_DATA		P2.3				SEL-1								INPUT
			SCL			P2.2				SEL-1								OUTPUT	LOW
			MISO		P2.1				SEL-1								INPUT
			MOSI		P2.0				SEL-1								OUTPUT	LOW

			CS_CFG		P7.0				OUTPUT	HIGH						OUTPUT	HIGH

			IRQ0		P1.0				INPUT								INPUT
			IRQ1		P4.3				INPUT								INPUT

			DATA		P7.2				INPUT								INPUT
			PLL_LOCK	P7.1				INPUT								INPUT
			CLKOUT		P7.3				INPUT								INPUT

	AD CHANNELS:
			C1 ��ѹ					P1.2		A2		SEL-3								SEL-3
			C2 ��ѹ					P1.3		A3		SEL-3								SEL-3

			��ȡ��ɣ�����1��		P9.0		A8		SEL-3								SEL-3
			��ȡ��ɣ�����2��		P9.1		A9		SEL-3								SEL-3

			��չ1					P9.2		A10		SEL-3								SEL-3
			��չ2					P9.3		A11		SEL-3								SEL-3

			���ٶ�X��				P9.4		A12		SEL-3								SEL-3
			���ٶ�Y��				P9.5		A13		SEL-3								SEL-3
			���ٶ�Z��				P9.6		A14		SEL-3								SEL-3
			�ն�					P9.7		A15		SEL-3								SEL-3

	RTC:
			SCL						P3.2				SEL-1								INPUT
			SDA						P3.1				SEL-1								INPUT
			RTC�ϵ�					P7.5				OUTPUT	LOW							OUTPUT	LOW
			RTC�ж�					P4.2				INPUT								INPUT

	�ܺĲ��������
			�ܺĲ���ʹ��				P3.0			OUTPUT	LOW							OUTPUT	LOW
			�ܺĲ���ʹ����ѹ			P7.6			OUTPUT	LOW		PFM/T0		SEL-1	OUTPUT
			��ɼ�����0������1��		P6.0			OUTPUT	LOW							OUTPUT	LOW
			��ɼ�����0������2��		P2.7			OUTPUT	LOW							OUTPUT	LOW
			��������C1��ѹ			P2.6			OUTPUT	LOW							OUTPUT	LOW
			��������C2��ѹ			P2.5			OUTPUT	LOW							OUTPUT	LOW


			C2���			P7.7			OUTPUT	LOW		PFM/T1		SEL-2	OUTPUT
			C2�ŵ�			P6.7			OUTPUT	LOW							OUTPUT	LOW

			C2�ŵ縺ѹ��		P3.3			OUTPUT	LOW		PFM/T1		SEL-2	OUTPUT

			�ϵ�Ԥ���ж�		P1.1			INPUT								INPUT

	������
			������1ʹ��		P2.4			OUTPUT	LOW							OUTPUT	LOW
			������2ʹ��		P6.1			OUTPUT	LOW							OUTPUT	LOW

			������			P6.2			OUTPUT	LOW							OUTPUT	LOW
			LED��			P4.5			OUTPUT	LOW							OUTPUT	LOW
			����			P4.4			INPUT								INPUT


			LFOSC			PJ.4			J.4	SEL-1
			LFOSC			PJ.5			J.4	SEL-1
			HFOSC			PJ.6			J.6	SEL-1
			HFOSC			PJ.7			J.6	SEL-1

			JTAG			PJ.0			OUTPUT	LOW
			JTAG			PJ.1			OUTPUT	LOW
			JTAG			PJ.2			OUTPUT	LOW
			JTAG			PJ.3			OUTPUT	LOW
*/
