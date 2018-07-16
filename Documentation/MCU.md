/*
	系统硬件初始化 v1.0
	2014.12.16

	启动时间：近190mS


系统硬件资源配置：	430FR6979IPN

	时钟系统：
ACLK:	32K OSC		/1
MCLK:	4MHz DCO	/1		可选外置晶振
SMCLK:	MCLK		/1

	LCD：
		段0-段7，其余未使用
		ACLK，64Hz，3.02V

	USCI:
		A0：	射频通信		（SPI）
		B0：	IO被LCD占用
		A1：	诊断屏			（SPI）
		B1：	RTC、EEPROM		(IIC)

	定时器：
		TA0.0:	电压泵，		ACLK/1，	周期可变，	按需工作，中断关
		TA0.1:	电压泵输出时钟
		TA0.2:

		TA1.0:	C2充放电控制，	ACLK/8，	周期可变，	间断工作，中断关
		TA1.1:	C2放电负压泵
		TA1.2:	C2充电PFM输出

		TA2.0:	通信DMA触发，	SMCLK/4，	10循环，	间断工作，中断关
		TA2.1:

		TA3.0：	系统RTC，		ACLK/8，	4096循环，	持续工作，中断开		(启动时用于等待晶振起振)
		TA3.1:	短时间休眠唤醒（临时使用）
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
		通道0:	射频通信，单次，触发源 DMA0TSEL__TA2CCR0
		通道1:
		通道2:	诊断显示屏，单次，触发源 DMA2TSEL__UCA1TXIFG

	AD:



	IO分配见下表。

*/


//	硬件与底层区
#include <hal_board.h>



sysStat sys_status;							// 系统状态
uint32 HAL_CPU_CLOCK_Hz;					// 主频. 单位：Hz
uint16 HAL_CPU_CLOCK_KHz;					// 主频. 单位：KHz. up to 65M
uint16 HAL_CPU_CLOCK_DELAY;					// 延时函数专用，不必理睬

bool sync_flag;								// 同步标志


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
			C1 电压					P1.2		A2		SEL-3								SEL-3
			C2 电压					P1.3		A3		SEL-3								SEL-3

			获取电荷（量程1）		P9.0		A8		SEL-3								SEL-3
			获取电荷（量程2）		P9.1		A9		SEL-3								SEL-3

			扩展1					P9.2		A10		SEL-3								SEL-3
			扩展2					P9.3		A11		SEL-3								SEL-3

			加速度X轴				P9.4		A12		SEL-3								SEL-3
			加速度Y轴				P9.5		A13		SEL-3								SEL-3
			加速度Z轴				P9.6		A14		SEL-3								SEL-3
			照度					P9.7		A15		SEL-3								SEL-3

	RTC:
			SCL						P3.2				SEL-1								INPUT
			SDA						P3.1				SEL-1								INPUT
			RTC上电					P7.5				OUTPUT	LOW							OUTPUT	LOW
			RTC中断					P4.2				INPUT								INPUT

	能耗测量与管理：
			能耗测量使能				P3.0			OUTPUT	LOW							OUTPUT	LOW
			能耗测量使能升压			P7.6			OUTPUT	LOW		PFM/T0		SEL-1	OUTPUT
			电荷计量清0（量程1）		P6.0			OUTPUT	LOW							OUTPUT	LOW
			电荷计量清0（量程2）		P2.7			OUTPUT	LOW							OUTPUT	LOW
			测量电容C1电压			P2.6			OUTPUT	LOW							OUTPUT	LOW
			测量电容C2电压			P2.5			OUTPUT	LOW							OUTPUT	LOW


			C2充电			P7.7			OUTPUT	LOW		PFM/T1		SEL-2	OUTPUT
			C2放电			P6.7			OUTPUT	LOW							OUTPUT	LOW

			C2放电负压泵		P3.3			OUTPUT	LOW		PFM/T1		SEL-2	OUTPUT

			断电预警中断		P1.1			INPUT								INPUT

	其它：
			传感器1使能		P2.4			OUTPUT	LOW							OUTPUT	LOW
			传感器2使能		P6.1			OUTPUT	LOW							OUTPUT	LOW

			蜂鸣器			P6.2			OUTPUT	LOW							OUTPUT	LOW
			LED灯			P4.5			OUTPUT	LOW							OUTPUT	LOW
			按键			P4.4			INPUT								INPUT


			LFOSC			PJ.4			J.4	SEL-1
			LFOSC			PJ.5			J.4	SEL-1
			HFOSC			PJ.6			J.6	SEL-1
			HFOSC			PJ.7			J.6	SEL-1

			JTAG			PJ.0			OUTPUT	LOW
			JTAG			PJ.1			OUTPUT	LOW
			JTAG			PJ.2			OUTPUT	LOW
			JTAG			PJ.3			OUTPUT	LOW
*/
