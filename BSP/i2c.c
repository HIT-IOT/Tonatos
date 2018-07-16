#include <msp430.h>
#include "inc/hw_regaccess.h"
#include "inc/hw_memmap.h"

#include "eusci_b_i2c.h"

#include <assert.h>

void EUSCI_B_I2C_slaveDataPut(uint16_t baseAddress,
                              uint8_t transmitData
                              )
{
        //Send single byte data.
        HWREG16(baseAddress + OFS_UCBxTXBUF) = transmitData;
}
uint8_t EUSCI_B_I2C_slaveDataGet(uint16_t baseAddress)
{
        //Read a byte.
        return HWREG16(baseAddress + OFS_UCBxRXBUF);
}

void EUSCI_B_I2C_masterInit(uint16_t baseAddress,
                            uint8_t selectClockSource,
                            uint32_t i2cClk,
                            uint32_t dataRate,
                            uint8_t byteCounterThreshold,
                            uint8_t autoSTOPGeneration
                            )
{
        EUSCI_B_I2C_initMasterParam param = { 0 };

        param.selectClockSource = selectClockSource;
        param.i2cClk = i2cClk;
        param.dataRate = dataRate;
        param.byteCounterThreshold = byteCounterThreshold;
        param.autoSTOPGeneration = autoSTOPGeneration;

        EUSCI_B_I2C_initMaster(baseAddress, &param);
}

void EUSCI_B_I2C_clearInterruptFlag(uint16_t baseAddress,
                                    uint16_t mask
                                    )
{
        assert( 0x00 == ( mask & ~(EUSCI_B_I2C_STOP_INTERRUPT +
                                   EUSCI_B_I2C_START_INTERRUPT +
                                   EUSCI_B_I2C_NAK_INTERRUPT +
                                   EUSCI_B_I2C_ARBITRATIONLOST_INTERRUPT +
                                   EUSCI_B_I2C_BIT9_POSITION_INTERRUPT +
                                   EUSCI_B_I2C_CLOCK_LOW_TIMEOUT_INTERRUPT +
                                   EUSCI_B_I2C_BYTE_COUNTER_INTERRUPT +
                                   EUSCI_B_I2C_TRANSMIT_INTERRUPT0 +
                                   EUSCI_B_I2C_TRANSMIT_INTERRUPT1 +
                                   EUSCI_B_I2C_TRANSMIT_INTERRUPT2 +
                                   EUSCI_B_I2C_TRANSMIT_INTERRUPT3 +
                                   EUSCI_B_I2C_RECEIVE_INTERRUPT0 +
                                   EUSCI_B_I2C_RECEIVE_INTERRUPT1 +
                                   EUSCI_B_I2C_RECEIVE_INTERRUPT2 +
                                   EUSCI_B_I2C_RECEIVE_INTERRUPT3
                                   ))
                );
        //Clear the I2C interrupt source.
        HWREG16(baseAddress + OFS_UCBxIFG) &= ~(mask);
}
void EUSCI_B_I2C_masterMultiByteSendStop(uint16_t baseAddress)
{
        //If interrupts are not used, poll for flags
//        if (!(HWREG16(baseAddress + OFS_UCBxIE) & UCTXIE))
//                //Poll for transmit interrupt flag.
//                while (!(HWREG16(baseAddress + OFS_UCBxIFG) & UCTXIFG)) ;

        //Send stop condition.
        HWREG16(baseAddress + OFS_UCBxCTLW0) |= UCTXSTP;
}
void EUSCI_B_I2C_masterMultiByteReceiveStop(uint16_t baseAddress)
{
        //Send stop condition.
        HWREG16(baseAddress + OFS_UCBxCTLW0) |= UCTXSTP;
}
