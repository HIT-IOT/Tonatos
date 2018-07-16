#include "inc/hw_regaccess.h"
#include "inc/hw_memmap.h"

//#ifdef __MSP430_HAS_EUSCI_Ax__
#include "eusci_a_spi.h"

#include <assert.h>

void EUSCI_A_SPI_clearInterruptFlag(uint16_t baseAddress,
                                    uint8_t mask
                                    )
{
        assert(!(mask & ~(EUSCI_A_SPI_RECEIVE_INTERRUPT
                          | EUSCI_A_SPI_TRANSMIT_INTERRUPT)));

        HWREG16(baseAddress + OFS_UCAxIFG) &=  ~mask;
}


void EUSCI_A_SPI_setInterruptFlag(uint16_t baseAddress,
                                    uint8_t mask
                                    )
{
        assert(!(mask & ~(EUSCI_A_SPI_RECEIVE_INTERRUPT
                          | EUSCI_A_SPI_TRANSMIT_INTERRUPT)));

        HWREG16(baseAddress + OFS_UCAxIFG) |=  mask;
}

void EUSCI_A_SPI_masterInit(uint16_t baseAddress,
                            uint8_t selectClockSource,
                            uint32_t clockSourceFrequency,
                            uint32_t desiredSpiClock,
                            uint16_t msbFirst,
                            uint16_t clockPhase,
                            uint16_t clockPolarity,
                            uint16_t spiMode
                            )
{
        EUSCI_A_SPI_initMasterParam param = { 0 };

        param.selectClockSource = selectClockSource;
        param.clockSourceFrequency = clockSourceFrequency;
        param.desiredSpiClock = desiredSpiClock;
        param.msbFirst = msbFirst;
        param.clockPhase = clockPhase;
        param.clockPolarity = clockPolarity;
        param.spiMode = spiMode;

        EUSCI_A_SPI_initMaster(baseAddress, &param);
}

//#endif
