#include <macros.h>
#include <msp430.h>
#include <hal_beeper.h>

void beep (uint16 bp_time)
{
    int i;
    MCU_IO_OUTPUT (HAL_BOARD_IO_BEEP_PORT,HAL_BOARD_IO_BEEP_PIN,0);
    for (i=0; i<bp_time; i++)
    {
        halMcuWaitUs(233);
        MCU_IO_TGL (HAL_BOARD_IO_BEEP_PORT,HAL_BOARD_IO_BEEP_PIN);
    }

    MCU_IO_OUTPUT (HAL_BOARD_IO_BEEP_PORT,HAL_BOARD_IO_BEEP_PIN,0);
}
