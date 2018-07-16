#include <dma.h>
#include <types.h>

extern bool    DMA_WAKE_INT;
uint8_t DMA_IN_USE(uint8_t channelSelect);

void DMA_Select_trigger_Source(uint8_t channelSelect,
                         uint16_t Source);

bool DMA_initialize(uint8_t channelSelect,
              uint16_t transferModeSelect,
              uint16_t transferSize,
              uint8_t triggerSourceSelect,
              uint8_t transferUnitSelect,
              uint8_t triggerTypeSelect);
void DMA_installInterrupt(uint8_t channelSelect, ISR_FUNC_PTR channelFunc);
