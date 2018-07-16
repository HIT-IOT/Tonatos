#include <eusci_a_spi.h>

extern void EUSCI_A_SPI_clearInterruptFlag(uint16_t baseAddress,
                                           uint8_t mask);

extern void EUSCI_A_SPI_setInterruptFlag(uint16_t baseAddress,
                                           uint8_t mask);


extern void EUSCI_A_SPI_masterInit(uint16_t baseAddress,
                                   uint8_t selectClockSource,
                                   uint32_t clockSourceFrequency,
                                   uint32_t desiredSpiClock,
                                   uint16_t msbFirst,
                                   uint16_t clockPhase,
                                   uint16_t clockPolarity,
                                   uint16_t spiMode);
