#include <eusci_b_i2c.h>

extern void EUSCI_B_I2C_slaveDataPut(uint16_t baseAddress,
                                     uint8_t transmitData);

extern uint8_t EUSCI_B_I2C_slaveDataGet(uint16_t baseAddress);


extern void EUSCI_B_I2C_clearInterruptFlag(uint16_t baseAddress,
                                           uint16_t mask);

extern void EUSCI_B_I2C_masterMultiByteSendStop(uint16_t baseAddress);


extern void EUSCI_B_I2C_masterMultiByteReceiveStop(uint16_t baseAddress);




extern void EUSCI_B_I2C_masterInit(uint16_t baseAddress,
                                   uint8_t selectClockSource,
                                   uint32_t i2cClk,
                                   uint32_t dataRate,
                                   uint8_t byteCounterThreshold,
                                   uint8_t autoSTOPGeneration);

