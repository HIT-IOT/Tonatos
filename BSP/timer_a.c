
#include "timer_a.h"

#include <assert.h>

//*****************************************************************************
//
//! \brief Private clock source divider helper function
//!
//! \param baseAddress is the base address of the TIMER_A module.
//! \param clockSourceDivider is the desired divider for the clock source
//!        Valid values are:
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_1 [Default]
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_2
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_4
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_8
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_3
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_5
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_6
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_7
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_10
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_12
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_14
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_16
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_20
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_24
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_28
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_32
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_40
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_48
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_56
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_64
//!
//! Modified bits of \b TAxCTL register and bits of \b TAxEX0 register.
//!
//! \return None
//
//*****************************************************************************
static void privateTimerAProcessClockSourceDivider(uint32_t baseAddress,
                                                   uint16_t clockSourceDivider)
{
        HWREG16(baseAddress + OFS_TAxCTL) &= ~ID__8;
        HWREG16(baseAddress + OFS_TAxEX0) &= ~TAIDEX_7;
        switch (clockSourceDivider) {
        case TIMER_A_CLOCKSOURCE_DIVIDER_1:
        case TIMER_A_CLOCKSOURCE_DIVIDER_2:
                HWREG16(baseAddress + OFS_TAxCTL) |= ((clockSourceDivider - 1) << 6);
                HWREG16(baseAddress + OFS_TAxEX0) = TAIDEX_0;
                break;
        case TIMER_A_CLOCKSOURCE_DIVIDER_4:
                HWREG16(baseAddress + OFS_TAxCTL) |= ID__4;
                HWREG16(baseAddress + OFS_TAxEX0) = TAIDEX_0;
                break;
        case TIMER_A_CLOCKSOURCE_DIVIDER_8:
                HWREG16(baseAddress + OFS_TAxCTL) |= ID__8;
                HWREG16(baseAddress + OFS_TAxEX0) = TAIDEX_0;
                break;
        case TIMER_A_CLOCKSOURCE_DIVIDER_3:
        case TIMER_A_CLOCKSOURCE_DIVIDER_5:
        case TIMER_A_CLOCKSOURCE_DIVIDER_6:
        case TIMER_A_CLOCKSOURCE_DIVIDER_7:
                HWREG16(baseAddress + OFS_TAxCTL) |= ID__1;
                HWREG16(baseAddress + OFS_TAxEX0) = (clockSourceDivider - 1);
                break;

        case TIMER_A_CLOCKSOURCE_DIVIDER_10:
        case TIMER_A_CLOCKSOURCE_DIVIDER_12:
        case TIMER_A_CLOCKSOURCE_DIVIDER_14:
        case TIMER_A_CLOCKSOURCE_DIVIDER_16:
                HWREG16(baseAddress + OFS_TAxCTL) |= ID__2;
                HWREG16(baseAddress + OFS_TAxEX0) = (clockSourceDivider / 2 - 1 );
                break;

        case TIMER_A_CLOCKSOURCE_DIVIDER_20:
        case TIMER_A_CLOCKSOURCE_DIVIDER_24:
        case TIMER_A_CLOCKSOURCE_DIVIDER_28:
        case TIMER_A_CLOCKSOURCE_DIVIDER_32:
                HWREG16(baseAddress + OFS_TAxCTL) |= ID__4;
                HWREG16(baseAddress + OFS_TAxEX0) = (clockSourceDivider / 4 - 1);
                break;
        case TIMER_A_CLOCKSOURCE_DIVIDER_40:
        case TIMER_A_CLOCKSOURCE_DIVIDER_48:
        case TIMER_A_CLOCKSOURCE_DIVIDER_56:
        case TIMER_A_CLOCKSOURCE_DIVIDER_64:
                HWREG16(baseAddress + OFS_TAxCTL) |= ID__8;
                HWREG16(baseAddress + OFS_TAxEX0) = (clockSourceDivider / 8 - 1);
                break;
        }
}
//*****************************************************************************
//
//! \brief Starts TIMER_A counter
//!
//! This function assumes that the timer has been previously configured using
//! TIMER_A_configureContinuousMode, TIMER_A_configureUpMode or
//! TIMER_A_configureUpDownMode.
//!
//! \param baseAddress is the base address of the TIMER_A module.
//! \param timerMode mode to put the timer in
//!        Valid values are:
//!        - \b TIMER_A_STOP_MODE
//!        - \b TIMER_A_UP_MODE
//!        - \b TIMER_A_CONTINUOUS_MODE [Default]
//!        - \b TIMER_A_UPDOWN_MODE
//!
//! Modified bits of \b TAxCTL register.
//!
//! \return None
//
//*****************************************************************************
void TIMER_A_startCounter( uint32_t baseAddress,
                           uint16_t timerMode
                           )
{
        assert(
                (TIMER_A_UPDOWN_MODE == timerMode) ||
                (TIMER_A_CONTINUOUS_MODE == timerMode) ||
                (TIMER_A_UP_MODE == timerMode)
                );

        HWREG16(baseAddress + OFS_TAxCTL) |= timerMode;
}


//*****************************************************************************
//
//! \brief Generate a PWM with timer running in up mode
//!
//! \param baseAddress is the base address of the TIMER_A module.
//! \param clockSource selects Clock source.
//!        Valid values are:
//!        - \b TIMER_A_CLOCKSOURCE_EXTERNAL_TXCLK [Default]
//!        - \b TIMER_A_CLOCKSOURCE_ACLK
//!        - \b TIMER_A_CLOCKSOURCE_SMCLK
//!        - \b TIMER_A_CLOCKSOURCE_INVERTED_EXTERNAL_TXCLK
//! \param clockSourceDivider is the desired divider for the clock source
//!        Valid values are:
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_1 [Default]
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_2
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_4
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_8
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_3
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_5
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_6
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_7
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_10
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_12
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_14
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_16
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_20
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_24
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_28
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_32
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_40
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_48
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_56
//!        - \b TIMER_A_CLOCKSOURCE_DIVIDER_64
//! \param timerPeriod selects the desired timer period
//! \param compareRegister selects the compare register being used. Refer to
//!        datasheet to ensure the device has the capture compare register
//!        being used.
//!        Valid values are:
//!        - \b TIMER_A_CAPTURECOMPARE_REGISTER_0
//!        - \b TIMER_A_CAPTURECOMPARE_REGISTER_1
//!        - \b TIMER_A_CAPTURECOMPARE_REGISTER_2
//!        - \b TIMER_A_CAPTURECOMPARE_REGISTER_3
//!        - \b TIMER_A_CAPTURECOMPARE_REGISTER_4
//!        - \b TIMER_A_CAPTURECOMPARE_REGISTER_5
//!        - \b TIMER_A_CAPTURECOMPARE_REGISTER_6
//! \param compareOutputMode specifies the output mode.
//!        Valid values are:
//!        - \b TIMER_A_OUTPUTMODE_OUTBITVALUE [Default]
//!        - \b TIMER_A_OUTPUTMODE_SET
//!        - \b TIMER_A_OUTPUTMODE_TOGGLE_RESET
//!        - \b TIMER_A_OUTPUTMODE_SET_RESET
//!        - \b TIMER_A_OUTPUTMODE_TOGGLE
//!        - \b TIMER_A_OUTPUTMODE_RESET
//!        - \b TIMER_A_OUTPUTMODE_TOGGLE_SET
//!        - \b TIMER_A_OUTPUTMODE_RESET_SET
//! \param dutyCycle specifies the dutycycle for the generated waveform
//!
//! Modified bits of \b TAxCTL register, bits of \b TAxCCTL0 register, bits of
//! \b TAxCCR0 register and bits of \b TAxCCTLn register.
//!
//! \return None
//
//*****************************************************************************
void TIMER_A_generatePWM(  uint32_t baseAddress,
                           uint16_t clockSource,
                           uint16_t clockSourceDivider,
                           uint16_t timerPeriod,
                           uint16_t compareRegister,
                           uint16_t compareOutputMode,
                           uint16_t dutyCycle
                           )
{
        assert(
                (TIMER_A_CLOCKSOURCE_EXTERNAL_TXCLK == clockSource) ||
                (TIMER_A_CLOCKSOURCE_ACLK == clockSource) ||
                (TIMER_A_CLOCKSOURCE_SMCLK == clockSource) ||
                (TIMER_A_CLOCKSOURCE_INVERTED_EXTERNAL_TXCLK == clockSource)
                );

        assert((TIMER_A_CAPTURECOMPARE_REGISTER_0 == compareRegister) ||
               (TIMER_A_CAPTURECOMPARE_REGISTER_1 == compareRegister) ||
               (TIMER_A_CAPTURECOMPARE_REGISTER_2 == compareRegister) ||
               (TIMER_A_CAPTURECOMPARE_REGISTER_3 == compareRegister) ||
               (TIMER_A_CAPTURECOMPARE_REGISTER_4 == compareRegister) ||
               (TIMER_A_CAPTURECOMPARE_REGISTER_5 == compareRegister) ||
               (TIMER_A_CAPTURECOMPARE_REGISTER_6 == compareRegister)
               );

        assert((TIMER_A_OUTPUTMODE_OUTBITVALUE == compareOutputMode) ||
               (TIMER_A_OUTPUTMODE_SET == compareOutputMode) ||
               (TIMER_A_OUTPUTMODE_TOGGLE_RESET == compareOutputMode) ||
               (TIMER_A_OUTPUTMODE_SET_RESET == compareOutputMode) ||
               (TIMER_A_OUTPUTMODE_TOGGLE == compareOutputMode) ||
               (TIMER_A_OUTPUTMODE_RESET == compareOutputMode) ||
               (TIMER_A_OUTPUTMODE_TOGGLE_SET == compareOutputMode) ||
               (TIMER_A_OUTPUTMODE_RESET_SET == compareOutputMode)
               );

        privateTimerAProcessClockSourceDivider(baseAddress,
                                               clockSourceDivider
                                               );

        HWREG16(baseAddress + OFS_TAxCTL)  &=
                ~( TIMER_A_CLOCKSOURCE_INVERTED_EXTERNAL_TXCLK +
                   TIMER_A_UPDOWN_MODE + TIMER_A_DO_CLEAR +
                   TIMER_A_TAIE_INTERRUPT_ENABLE
                   );

        HWREG16(baseAddress + OFS_TAxCTL)  |= ( clockSource +
                                                TIMER_A_UP_MODE +
                                                TIMER_A_DO_CLEAR
                                                );

        HWREG16(baseAddress + OFS_TAxCCR0)  = timerPeriod;

        HWREG16(baseAddress + OFS_TAxCCTL0)  &=
                ~(TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE +
                  TIMER_A_OUTPUTMODE_RESET_SET
                  );
        HWREG16(baseAddress + compareRegister)  |= compareOutputMode;

        HWREG16(baseAddress + compareRegister + OFS_TAxR) = dutyCycle;
}

void TIMER_A_clearCaptureCompareInterruptFlag(uint32_t baseAddress,
                                              uint16_t captureCompareRegister
                                              )
{
        assert((TIMER_A_CAPTURECOMPARE_REGISTER_0 == captureCompareRegister) ||
               (TIMER_A_CAPTURECOMPARE_REGISTER_1 == captureCompareRegister) ||
               (TIMER_A_CAPTURECOMPARE_REGISTER_2 == captureCompareRegister) ||
               (TIMER_A_CAPTURECOMPARE_REGISTER_3 == captureCompareRegister) ||
               (TIMER_A_CAPTURECOMPARE_REGISTER_4 == captureCompareRegister) ||
               (TIMER_A_CAPTURECOMPARE_REGISTER_5 == captureCompareRegister) ||
               (TIMER_A_CAPTURECOMPARE_REGISTER_6 == captureCompareRegister)
               );

        HWREG16(baseAddress + captureCompareRegister)  &= ~CCIFG;
}

void TIMER_A_initCompare(  uint32_t baseAddress,
                           uint16_t compareRegister,
                           uint16_t compareInterruptEnable,
                           uint16_t compareOutputMode,
                           uint16_t compareValue
                           )
{
        assert((TIMER_A_CAPTURECOMPARE_REGISTER_0 == compareRegister) ||
               (TIMER_A_CAPTURECOMPARE_REGISTER_1 == compareRegister) ||
               (TIMER_A_CAPTURECOMPARE_REGISTER_2 == compareRegister) ||
               (TIMER_A_CAPTURECOMPARE_REGISTER_3 == compareRegister) ||
               (TIMER_A_CAPTURECOMPARE_REGISTER_4 == compareRegister) ||
               (TIMER_A_CAPTURECOMPARE_REGISTER_5 == compareRegister) ||
               (TIMER_A_CAPTURECOMPARE_REGISTER_6 == compareRegister)
               );

        assert((TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE == compareInterruptEnable) ||
               (TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE == compareInterruptEnable)
               );

        assert((TIMER_A_OUTPUTMODE_OUTBITVALUE == compareOutputMode) ||
               (TIMER_A_OUTPUTMODE_SET == compareOutputMode) ||
               (TIMER_A_OUTPUTMODE_TOGGLE_RESET == compareOutputMode) ||
               (TIMER_A_OUTPUTMODE_SET_RESET == compareOutputMode) ||
               (TIMER_A_OUTPUTMODE_TOGGLE == compareOutputMode) ||
               (TIMER_A_OUTPUTMODE_RESET == compareOutputMode) ||
               (TIMER_A_OUTPUTMODE_TOGGLE_SET == compareOutputMode) ||
               (TIMER_A_OUTPUTMODE_RESET_SET == compareOutputMode)
               );

        if (TIMER_A_CAPTURECOMPARE_REGISTER_0 == compareRegister) {
                //CaptureCompare register 0 only supports certain modes
                assert((TIMER_A_OUTPUTMODE_OUTBITVALUE == compareOutputMode) ||
                       (TIMER_A_OUTPUTMODE_SET == compareOutputMode) ||
                       (TIMER_A_OUTPUTMODE_TOGGLE == compareOutputMode) ||
                       (TIMER_A_OUTPUTMODE_RESET == compareOutputMode)
                       );
        }

        HWREG16(baseAddress + compareRegister ) &=   ~CAP;

        HWREG16(baseAddress + compareRegister) &=
                ~(TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE +
                  TIMER_A_OUTPUTMODE_RESET_SET
                  );

        HWREG16(baseAddress + compareRegister)  |= ( compareInterruptEnable +
                                                     compareOutputMode
                                                     );

        HWREG16(baseAddress + compareRegister + OFS_TAxR) = compareValue;
}

uint32_t TIMER_A_getCaptureCompareInterruptStatus(uint32_t baseAddress,
                                                  uint16_t captureCompareRegister,
                                                  uint16_t mask
                                                  )
{
        return HWREG16(baseAddress + captureCompareRegister) & mask;
}
