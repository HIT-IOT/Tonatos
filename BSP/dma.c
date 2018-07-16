/* --COPYRIGHT--,BSD
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//*****************************************************************************
//
// dma.c - Driver for the dma Module.
//
// 引用头文件变动，增加中断处理函数，并可在使能中断时配置。中断结束后CPU恢复原睡眠状态。
// 增加功能：重新选择触发源     DMA_Select_trigger_Source
// 增加功能：检测DMA是否进行中  DMA_IN_USE
// 2016.12.17
//
//*****************************************************************************
#include <msp430.h>
#include "inc/hw_regaccess.h"
#include "inc/hw_memmap.h"

#if defined(__MSP430_HAS_DMAX_3__) || defined(__MSP430_HAS_DMAX_6__)
#include "dma.h"

#include <assert.h>
#include <types.h>
#include <defs.h>


bool    DMA_WAKE_INT = FALSE;

static  ISR_FUNC_PTR DMA_isr_tbl[7] = {0};

void DMA_initialize(uint8_t channelSelect,
              uint16_t transferModeSelect,
              uint16_t transferSize,
              uint8_t triggerSourceSelect,
              uint8_t transferUnitSelect,
              uint8_t triggerTypeSelect)
{
        DMA_initParam param = { 0 };

        param.channelSelect = channelSelect;
        param.transferModeSelect = transferModeSelect;
        param.transferSize = transferSize;
        param.triggerSourceSelect = triggerSourceSelect;
        param.transferUnitSelect = transferUnitSelect;
        param.triggerTypeSelect = triggerTypeSelect;

        DMA_init(&param);
}

void DMA_installInterrupt(uint8_t channelSelect, ISR_FUNC_PTR channelFunc)
{
        assert(channelSelect <= DMA_CHANNEL_7);

        DMA_isr_tbl[channelSelect >>4] = channelFunc;

        HWREG16(DMA_BASE + channelSelect + OFS_DMA0CTL) |= DMAIE;
}

void DMA_Select_trigger_Source(uint8_t channelSelect,
                         uint16_t Source)
{
        uint8_t triggerOffset = (channelSelect >> 4);

        if (triggerOffset & 0x01) {                                     //Odd Channel
                HWREG16(DMA_BASE + (triggerOffset & 0x0E)) &= 0x00FF;   //Reset Trigger Select
                HWREG16(DMA_BASE +
                        (triggerOffset & 0x0E)) |= (Source << 8);
        } else {                                                        //Even Channel
                HWREG16(DMA_BASE + (triggerOffset & 0x0E)) &= 0xFF00;   //Reset Trigger Select
                HWREG16(DMA_BASE + (triggerOffset & 0x0E)) |= Source;
        }
}


uint8_t DMA_IN_USE(uint8_t channelSelect)
{
        if ((HWREG16(DMA_BASE + channelSelect + OFS_DMA0CTL) & DMAEN) !=0)  return (TRUE);
        else    return (FALSE);
}

//*****************************************************************************
//
//! DMA_ISR，For interrupt
//!
//! \return None
//
//*****************************************************************************
#pragma vector=DMA_VECTOR
interrupt void DMA_ISR(void)
{
    register uint8_t i = DMAIV >>1;
    if ((i!=0)&&(DMA_isr_tbl[i-1] != NULL)) (*DMA_isr_tbl[i-1])();

    if (DMA_WAKE_INT)   __low_power_mode_off_on_exit();                     // 退出DMA并唤醒CPU
    DMA_WAKE_INT = FALSE;
}


#endif
//*****************************************************************************
//
//! Close the doxygen group for dma_api
//! @}
//
//*****************************************************************************
