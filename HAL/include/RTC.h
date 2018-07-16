#ifndef _RTC_H
#define _RTC_H

#include <defs.h>
#include <types.h>

#define HAL_BOARD_RTC_PWR_PORT          6
#define HAL_BOARD_RTC_PWR_PIN           7           // RTC电源
#define HAL_BOARD_RTC_PWR_ON            1           // 开启
#define HAL_BOARD_RTC_PWR_OFF           0           // 关闭

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

#define HAL_BOARD_IIC_BUS_ERROR         0           // 总线错误
#define HAL_BOARD_RX6110_NOT_EXIST      0x02        // 芯片不存在
#define HAL_BOARD_RX6110_VLF_FAIL       0x04        // 芯片掉电
#define HAL_BOARD_RX6110_READY          0x01        // 芯片正常

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



#define     RTC_UP_BOUND        4096            // 计数上限                     1秒

/******************************************************************************
 * SystemClock  TA3, 4KHz,
*/

#define SystemClockinit()   st( TA3CCR0 = RTC_UP_BOUND -1;   TA3CTL = TASSEL_1 + ID_3 + MC_1 + TACLR + TAIE; )  // ACLK/8, up mode, interrupt enabled
#define SystemClockSync()   st( TA3CTL |= TACLR;    )   // 秒同步
#define GetSystemClock()    TA3R                        // 注意：运行时读取TAR可能有错误！！！最好投票！！！
#define Standard_1ms        4.096                       // 1毫秒内TAR加几次
#define SYS_SLEEP_PERIOD    1                           // 系统睡眠唤醒周期，单位秒，只可设为2的n次方整数，否则计时会有误差。此值大于1时定义
//  #define SYS_SLEEP_FREQ      1                       // 系统睡眠唤醒频率，周期的倒数。此值大于1时定义


#define BCD_to_num_8(x)   st( x = (x >>4)*10 + (x & 0x0F); )        // BCD码转数字(uint8 型)
#define num_to_BCD_8(x)   st( x = ((x/10)<<4) + (x%10); )           // 数字转BCD码(uint8 型)


extern void rtc_power_on (void);                        // RTC上电
extern void rtc_power_off (void);                       // RTC电池运行

extern uint16 rtc_valid_check (void);                   // 有效性检查
uint16 RX6110_dummy_read (void);                        // 芯片要求的时序，无实际意义，不必理会。返回FALSE代表总线错误
extern uint16 rtc_read (void);                          // 读取时钟
extern uint16 rtc_preset (void);                        // 设置时钟
extern uint16 rtc_alarm (uint32 _time);                 // 设置唤醒时间。调用前需确保RTC已上电40ms以上。参数为毫秒，误差可达20ms。不超过1020秒，即1020000
extern uint16 rtc_alarm_off (void);                 // 关闭唤醒。调用前需确保RTC已上电40ms以上
extern uint16 rtc_sync (void);                          // 同步RTC时钟。调用前需确保RTC已上电40ms以上。执行本指令后，IRQ2立即回到低电位
extern uint16 rtc_sync_en (void);                       // 在IRQ2上输出同步时钟。调用前需确保RTC已上电40ms以上
extern uint16 rtc_sync_dis (void);                      // 在IRQ2上关闭同步时钟。调用前需确保RTC已上电40ms以上

extern void rtc_run (void);

#endif
