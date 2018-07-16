#ifndef HAL_LED_H
#define HAL_LED_H



#define HAL_BOARD_IO_LED_PORT               4
#define HAL_BOARD_IO_LED_PIN                5


#define HAL_BOARD_IO_LED_ON                 1
#define HAL_BOARD_IO_LED_OFF                0


extern void led_on (void);
extern void led_off (void);
extern void led_tgl (void);

#endif
