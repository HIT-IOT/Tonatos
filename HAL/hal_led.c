//  LED ะกตฦ

/***********************************************************************************
* INCLUDES
*/
#include <macros.h>
#include <hal_led.h>
#include <msp430.h>

void led_on (void)
{
    MCU_IO_OUTPUT (HAL_BOARD_IO_LED_PORT, HAL_BOARD_IO_LED_PIN, HAL_BOARD_IO_LED_ON);
}

void led_off (void)
{
    MCU_IO_OUTPUT (HAL_BOARD_IO_LED_PORT, HAL_BOARD_IO_LED_PIN, HAL_BOARD_IO_LED_OFF);
}

void led_tgl (void)
{
    MCU_IO_TGL (HAL_BOARD_IO_LED_PORT, HAL_BOARD_IO_LED_PIN);
}

