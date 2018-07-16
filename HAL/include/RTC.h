#ifndef _RTC_H
#define _RTC_H

#include <defs.h>
#include <types.h>

#define HAL_BOARD_RTC_PWR_PORT          6
#define HAL_BOARD_RTC_PWR_PIN           7           // RTC��Դ
#define HAL_BOARD_RTC_PWR_ON            1           // ����
#define HAL_BOARD_RTC_PWR_OFF           0           // �ر�

#define HAL_BOARD_RTC_INT_PORT          4
#define HAL_BOARD_RTC_INT_PIN           2           // INT
#define HAL_BOARD_RTC_INT_PIN_BIT       BIT2

//  --------------------------------    IIC     --------------------------------

#define HAL_BOARD_IIC_BASE              EUSCI_B1_BASE

#define HAL_BOARD_IIC_SCL_PORT          3
#define HAL_BOARD_IIC_SCL_PIN           2           // SCL

#define HAL_BOARD_IIC_SDA_PORT          3
#define HAL_BOARD_IIC_SDA_PIN           1           // SDA


#define HAL_BOARD_RX6110_ADD            0x32        // 011 0010 R/W

#define HAL_BOARD_IIC_BUS_ERROR         0           // ���ߴ���
#define HAL_BOARD_RX6110_NOT_EXIST      0x02        // оƬ������
#define HAL_BOARD_RX6110_VLF_FAIL       0x04        // оƬ����
#define HAL_BOARD_RX6110_READY          0x01        // оƬ����

typedef union
{
    struct {
        uint32 time;
        uint32 date;
    };
    struct {
        uint8 second;
        uint8 minute;
        uint8 hour;
        uint8 hour_H;
        uint8 day;
        uint8 month;
        uint8 year;
        uint8 week;
    };
} RTCControl;

extern RTCControl rtc;



#define     RTC_UP_BOUND        4096            // ��������                     1��

/******************************************************************************
 * SystemClock  TA3, 4KHz,
*/

#define SystemClockinit()   st( TA3CCR0 = RTC_UP_BOUND -1;   TA3CTL = TASSEL_1 + ID_3 + MC_1 + TACLR + TAIE; )  // ACLK/8, up mode, interrupt enabled
#define SystemClockSync()   st( TA3CTL |= TACLR;    )   // ��ͬ��
#define GetSystemClock()    TA3R                        // ע�⣺����ʱ��ȡTAR�����д��󣡣������ͶƱ������
#define Standard_1ms        4.096                       // 1������TAR�Ӽ���
#define SYS_SLEEP_PERIOD    1                           // ϵͳ˯�߻������ڣ���λ�룬ֻ����Ϊ2��n�η������������ʱ��������ֵ����1ʱ����
//  #define SYS_SLEEP_FREQ      1                       // ϵͳ˯�߻���Ƶ�ʣ����ڵĵ�������ֵ����1ʱ����


#define BCD_to_num_8(x)   st( x = (x >>4)*10 + (x & 0x0F); )        // BCD��ת����(uint8 ��)
#define num_to_BCD_8(x)   st( x = ((x/10)<<4) + (x%10); )           // ����תBCD��(uint8 ��)


extern void rtc_power_on (void);                        // RTC�ϵ�
extern void rtc_power_off (void);                       // RTC�������

extern uint16 rtc_valid_check (void);                   // ��Ч�Լ��
uint16 RX6110_dummy_read (void);                        // оƬҪ���ʱ����ʵ�����壬������ᡣ����FALSE�������ߴ���
extern uint16 rtc_read (void);                          // ��ȡʱ��
extern uint16 rtc_preset (void);                        // ����ʱ��
extern uint16 rtc_alarm (uint32 _time);                 // ���û���ʱ�䡣����ǰ��ȷ��RTC���ϵ�40ms���ϡ�����Ϊ���룬���ɴ�20ms��������1020�룬��1020000
extern uint16 rtc_alarm_off (void);                 // �رջ��ѡ�����ǰ��ȷ��RTC���ϵ�40ms����
extern uint16 rtc_sync (void);                          // ͬ��RTCʱ�ӡ�����ǰ��ȷ��RTC���ϵ�40ms���ϡ�ִ�б�ָ���IRQ2�����ص��͵�λ
extern uint16 rtc_sync_en (void);                       // ��IRQ2�����ͬ��ʱ�ӡ�����ǰ��ȷ��RTC���ϵ�40ms����
extern uint16 rtc_sync_dis (void);                      // ��IRQ2�Ϲر�ͬ��ʱ�ӡ�����ǰ��ȷ��RTC���ϵ�40ms����

extern void rtc_run (void);

#endif
