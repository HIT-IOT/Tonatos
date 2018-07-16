#ifndef _430_SPI_SX1211
#define _430_SPI_SX1211


#include <hal_digio.h>
#include <diagnose.h>


// 硬件版本：FR6979 V2
/******************************************************************************
 * PHY Layer
 */
//  SX1211 SPI

#define HAL_SX1211_SPI_BASE             EUSCI_A0_BASE

#define HAL_SX1211_SPISCK_PORT                  2
#define HAL_SX1211_SPISCK_PIN                   2
#define HAL_SX1211_SPIMISO_PORT                 2
#define HAL_SX1211_SPIMISO_PIN                  1
#define HAL_SX1211_SPIMOSI_PORT                 2
#define HAL_SX1211_SPIMOSI_PIN                  0

//  SX1211 CTRL

#define HAL_SX1211_CS_DATA_PORT                 2
#define HAL_SX1211_CS_DATA_PIN                  3
#define HAL_SX1211_CS_CFG_PORT                  7
#define HAL_SX1211_CS_CFG_PIN                   0

//  SX1211 IO

#define HAL_SX1211_IRQ0_PORT                    3
#define HAL_SX1211_IRQ0_PIN                     3
#define HAL_SX1211_IRQ0_PIN_BIT                 BIT3
#define HAL_SX1211_IRQ1_PORT                    4
#define HAL_SX1211_IRQ1_PIN                     3
#define HAL_SX1211_IRQ1_PIN_BIT                 BIT3

#define HAL_SX1211_DATA_PORT                    7
#define HAL_SX1211_DATA_PIN                     2
#define HAL_SX1211_PLL_LOCK_PORT                7
#define HAL_SX1211_PLL_LOCK_PIN                 1
#define HAL_SX1211_CLKOUT_PORT                  7
#define HAL_SX1211_CLKOUT_PIN                   3


//  SX1211 Continuous Mode
#define HAL_SX1211_DCLK_PORT                    HAL_SX1211_IRQ1_PORT
#define HAL_SX1211_DCLK_PIN                     HAL_SX1211_IRQ1_PIN





#define     SX1211_CFG_CS()     st( MCU_IO_SET_LOW (HAL_SX1211_CS_CFG_PORT, HAL_SX1211_CS_CFG_PIN);     )
#define     SX1211_CFG_DE_CS()  st( MCU_IO_SET_HIGH (HAL_SX1211_CS_CFG_PORT, HAL_SX1211_CS_CFG_PIN);    )


extern const digioConfig pinRadio_IRQ0;
extern const digioConfig pinRadio_IRQ1;
extern const digioConfig pinRadio_PLL_LOCK;     // PLL_LOCK
extern const digioConfig pinRadio_CLKOUT;           // CLKOUT


extern uint16 Send_num, Rec_num;                    // 发送和接收长度，用于SPI通信方式
extern bool SX1211_SPI_BUSY;                        // SX1211 SPI 占用




extern void SX1211_SPI_Setup (void);                // SX1211   SPI、DMA初始化程序


extern void SX1211_Send_CFG_DMA (uint32 _srcAddress, uint16 _size);                     // SX1211发送命令，DMA模式
extern void SX1211_Send_Bytes_DMA (uint32 _srcAddress, uint16 _size);                       // SX1211发送字节，DMA模式
extern void SX1211_Send_CFG_SPI (uint32 _srcAddress, uint16 _size);                     // SX1211发送命令，SPI模式
extern void SX1211_Send_Bytes_SPI (uint32 _srcAddress, uint16 _size);                       // SX1211发送字节，SPI模式
extern void SX1211_Read_CFG_SPI (uint32 _srcAddress, uint32 _recAddress, uint16 _size);     // SX1211接收命令，SPI模式
extern void SX1211_Read_Bytes_SPI (uint32 _srcAddress, uint32 _recAddress, uint16 _size);   // SX1211接收字节，SPI模式


void DMA0_isr (void);

#endif
