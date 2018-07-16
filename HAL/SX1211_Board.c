/*
���ܣ�SX1211оƬ SPI��IO �������� For MSP430
���ߣ�����
��д���ڣ�2016.12.25

�������ڣ���д������DMA��SPI���ַ������䣬һ��ֻ��һ��������֧�ֶ����������䡣
���Ĺ��ܣ�2017.1.9
--------------------------------------------------------------------

������
�ó���Ϊ SX1211оƬ   SPI��IO ���ó���ʹ��USCI_A0
�ó���רΪSX1211оƬ���ơ�Ҳ��ʹ����SX1212��


���ԡ�ע�⣺
IO���ò���������󹦺ģ����ò��ֳ���Ҫ��Ӳ�����ƥ�䣡��
SPI���ݴ������ʲ��ɳ���1M��
SPI���ô������ʲ��ɳ���4M��
������ʹ��Ӳ��SPI����ʹ��DMA0���Խ�ʡ����
һ�η���5���ֽ����ϲ����Գ�DMA�����ƣ�������SPIֱ���ܸ���Щ��
ע��������δ���ǶԿ��Ź��Ĵ���

�ó�����ϲ�(MAC)���ã������ֶ����á�

ֻ��SPIģʽ���ܽ������ݡ�
��Ϊ�͹��ģ�����������������С�����δ����DMAģʽ���������������⡣


����SX1211Ҫ��ÿ��һ���ֽ�CS�������ߣ����ɲ���UCA0TXIFGΪDMA�Ĵ���Դ����������
��˲���TMRA2��ʱ����DMA��������˶�ʱ������ʮ�����У����������Ķ�SMCLK/ͨ�����ʺ�����������÷���ʱ�䣡�������������ô���ʱ�䣡��������




void SX1211_Send_CFG_DMA (uint32 _srcAddress, uint16 _size);                        // SX1211�������DMAģʽ
void SX1211_Send_Bytes_DMA (uint32 _srcAddress, uint16 _size);                      // SX1211�����ֽڣ�DMAģʽ
    DMAģʽ�������ڷ��ͣ��ʺ�5���ֽ����ϵ��������͡�׼��ʱ��63us��1���ֽ�11us


void SX1211_Send_CFG_SPI (uint32 _srcAddress, uint16 _size);                        // SX1211�������SPIģʽ
void SX1211_Send_Bytes_SPI (uint32 _srcAddress, uint16 _size);                      // SX1211�����ֽڣ�SPIģʽ
    SPIģʽ�������ڷ��ͣ��ʺ�5���ֽ����µ��������͡�׼��ʱ��16us��1���ֽ�21us


void SX1211_Rec_CFG_SPI (uint32 _srcAddress, uint32 _recAddress, uint16 _size);     // SX1211�������SPIģʽ
void SX1211_Rec_Bytes_SPI (uint32 _srcAddress, uint32 _recAddress, uint16 _size);   // SX1211�����ֽڣ�SPIģʽ
    SPIģʽ�����ڷ���/���ա�׼��ʱ��16us��1���ֽ�21us

*/

#include <bsp_dma.h>
#include <SX1211_Board.h>
#include <msp430.h>
#include <macros.h>
#include <spi.h>
#include <hal.h>

const digioConfig pinRadio_IRQ0 = {HAL_SX1211_IRQ0_PORT, HAL_SX1211_IRQ0_PIN, HAL_SX1211_IRQ0_PIN_BIT, HAL_DIGIO_INPUT,  0};        // IRQ0
const digioConfig pinRadio_IRQ1 = {HAL_SX1211_IRQ1_PORT, HAL_SX1211_IRQ1_PIN, HAL_SX1211_IRQ1_PIN_BIT, HAL_DIGIO_INPUT,  0};        // IRQ1

uint32 SendAddress, RecAddress;             // ���ͺͽ���ָ�룬����SPIͨ�ŷ�ʽ
uint16 Send_num, Rec_num;                   // ���ͺͽ��ճ��ȣ�����SPIͨ�ŷ�ʽ

bool SX1211_SPI_BUSY = TRUE;                // SX1211 SPI ռ��

void SX1211_SPI_Setup (void)                // ��ʼ��SX1211ͨ��
{
//  ��ʼ��SX1211   IO
    MCU_IO_INPUT (HAL_SX1211_DATA_PORT, HAL_SX1211_DATA_PIN, MCU_IO_PULLOFF);           // DATA����
    MCU_IO_INPUT (HAL_SX1211_PLL_LOCK_PORT, HAL_SX1211_PLL_LOCK_PIN, MCU_IO_PULLOFF);   // PLL_LOCK����
    MCU_IO_OUTPUT (HAL_SX1211_CS_CFG_PORT, HAL_SX1211_CS_CFG_PIN, 1);                   // NSS_CFG�����

    MCU_IO_PERIPHERAL_1 (HAL_SX1211_CS_DATA_PORT, HAL_SX1211_CS_DATA_PIN, MCU_IO_DIR_OUTPUT);   // UCA0STE
    MCU_IO_PERIPHERAL_1 (HAL_SX1211_SPISCK_PORT, HAL_SX1211_SPISCK_PIN, MCU_IO_DIR_OUTPUT);     // UCA0CLK
    MCU_IO_PERIPHERAL_1 (HAL_SX1211_SPIMOSI_PORT, HAL_SX1211_SPIMOSI_PIN, MCU_IO_DIR_OUTPUT);   // UCA0SIMO
    MCU_IO_PERIPHERAL_1 (HAL_SX1211_SPIMISO_PORT, HAL_SX1211_SPIMISO_PIN, MCU_IO_DIR_INPUT);    // UCA0SOMI


//  ��ʼ��SPI, A0Ϊ��ģʽ
    EUSCI_A_SPI_disable(HAL_SX1211_SPI_BASE);                               // �ر�SPI

    EUSCI_A_SPI_masterInit(HAL_SX1211_SPI_BASE,                             // UCA0
                           EUSCI_A_SPI_CLOCKSOURCE_SMCLK,                   // SMCLK
                           (uint32)HAL_CPU_CLOCK_KHz *1000,
                           1000000,                                         // 1M
                           EUSCI_A_SPI_MSB_FIRST,
                           EUSCI_A_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,
                           EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_LOW,
                           EUSCI_A_SPI_4PIN_UCxSTE_ACTIVE_LOW               // 4pin
                           );
    EUSCI_A_SPI_select4PinFunctionality(HAL_SX1211_SPI_BASE, EUSCI_A_SPI_ENABLE_SIGNAL_FOR_4WIRE_SLAVE);    // ʹ��CS����

//  ��ʼ��DMA��ͨ��0
        /*
         * Configure DMA channel 0
         * Configure channel for SINGLE transfers
         * DMA interrupt flag will be set after every 0 transfers
         * Use UCA0TXIFG to trigger transfers
         * Transfer BYTE-to-BYTE
         * Trigger upon Rising Edge of Trigger Source Signal
         */
    DMA_initialize(DMA_CHANNEL_0,
             DMA_TRANSFER_SINGLE,
             0,
             DMA0TSEL__TA2CCR0,                                             // ��˵������������
             DMA_SIZE_SRCBYTE_DSTBYTE,
             DMA_TRIGGER_RISINGEDGE);
        /*
         * DMA transfers inhibited during read-modify-write CPU operations
         */
    DMA_disableTransferDuringReadModifyWrite ();

    DMA_setDstAddress(DMA_CHANNEL_0, (uint32)&UCA0TXBUF, DMA_DIRECTION_UNCHANGED);  // ����DMA0Ŀ���ַ��UCA0TX����ַ����

    SX1211_SPI_BUSY = FALSE;                                                // ���SPIռ��
}


void SX1211_Send_CFG_DMA (uint32 _srcAddress, uint16 _size)             // SX1211�������DMAģʽ
{
    if (_size == 0)     return;                                             // �����ݣ��˳�

    EUSCI_A_SPI_disable(HAL_SX1211_SPI_BASE);                                       // �ر�SPI
    SX1211_SPI_BUSY = TRUE;                                                 // SPIռ��
    MCU_IO_OUTPUT (HAL_SX1211_CS_DATA_PORT, HAL_SX1211_CS_DATA_PIN, 1);     // NSS_DAT�����
    SX1211_CFG_CS();                                                        // CFG����Ƭѡ

//  ��ʼ��������                                                                                                          // ע���˴�����Դ������UCA0TXIFG����Ϊ�˼����ܺĿ�֧�����ٸ���
    DMA_setSrcAddress(DMA_CHANNEL_0, _srcAddress, DMA_DIRECTION_INCREMENT); // ����DMA0ԭ��ַ����ַ����
    DMA_setTransferSize(DMA_CHANNEL_0, _size);                              // ���ô���ߴ�
    DMA_clearInterrupt(DMA_CHANNEL_0);                                      // ���жϱ�־λ
    DMA_installInterrupt(DMA_CHANNEL_0, DMA0_isr);                           // ����DMA0�ж�
    DMA_enableTransfers(DMA_CHANNEL_0);                                     // ʹ��DMA0

    TA2CTL = 0;                                                             // �ر�TA2��ʱ
    TA2CCTL0 = 0;                                                           // TA2��CCR0����DMA��ʱ�� ���˵��������
    EUSCI_A_SPI_clearInterruptFlag(HAL_SX1211_SPI_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);    // ���־λ
    EUSCI_A_SPI_enable(HAL_SX1211_SPI_BASE);                                // ����SPI����ʼ����
    EUSCI_A_SPI_setInterruptFlag(HAL_SX1211_SPI_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);  // �ñ�־λ�����´���

//  ��ʼ��Timer A1������ʱ��Ϊ SPI����1���ֽڵ�ʱ�� * 1.3                                                ��˵������������
    TA2CCR0 = 10;
    TA2CTL = TASSEL_2 + ID_2 + MC_1 + TACLR;                                // SMCLK/4, up mode, interrupt disabled
}


void SX1211_Send_Bytes_DMA (uint32 _srcAddress, uint16 _size)           // SX1211�����ֽڣ�DMAģʽ
{
    if (_size == 0)     return;                                             // �����ݣ��˳�

    EUSCI_A_SPI_disable(HAL_SX1211_SPI_BASE);                               // �ر�SPI
    SX1211_SPI_BUSY = TRUE;                                                 // SPIռ��
    SX1211_CFG_DE_CS ();                                                    // �������֣�CS����


//  ��ʼ��������
    DMA_setSrcAddress(DMA_CHANNEL_0, _srcAddress, DMA_DIRECTION_INCREMENT); // ����DMA0ԭ��ַ����ַ����
    DMA_setTransferSize(DMA_CHANNEL_0, _size);                              // ���ô���ߴ�
    DMA_clearInterrupt(DMA_CHANNEL_0);                                      // ���жϱ�־λ
    DMA_installInterrupt(DMA_CHANNEL_0, DMA0_isr);                           // ����DMA0�ж�
    DMA_enableTransfers(DMA_CHANNEL_0);                                     // ʹ��DMA0


    TA2CTL = 0;                                                             // �ر�TA2��ʱ
    TA2CCTL0 = 0;                                                           // TA2��CCR0����DMA��ʱ�� ���˵��������
    MCU_IO_PERIPHERAL_1 (HAL_SX1211_CS_DATA_PORT, HAL_SX1211_CS_DATA_PIN, MCU_IO_DIR_OUTPUT);   // CSʹ��
    EUSCI_A_SPI_clearInterruptFlag(HAL_SX1211_SPI_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);        // ���־λ
    EUSCI_A_SPI_enable(HAL_SX1211_SPI_BASE);                                // ����SPI����ʼ����
    EUSCI_A_SPI_setInterruptFlag(HAL_SX1211_SPI_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);          // �ñ�־λ�����´���

//  ��ʼ��Timer A1������ʱ��Ϊ SPI����1���ֽڵ�ʱ�� * 1.3                                                ��˵������������
    TA2CCR0 = 10;
    TA2CTL = TASSEL_2 + ID_2 + MC_1 + TACLR;                                // SMCLK/4, up mode, interrupt disabled
}


void SX1211_Send_CFG_SPI (uint32 _srcAddress, uint16 _size)                 // SX1211�������SPIģʽ
{
    if (_size == 0)     return;                                             // �����ݣ��˳�

    EUSCI_A_SPI_disable(HAL_SX1211_SPI_BASE);                               // �ر�SPI
    SX1211_SPI_BUSY = TRUE;                                                 // SPIռ��
    MCU_IO_OUTPUT (HAL_SX1211_CS_DATA_PORT, HAL_SX1211_CS_DATA_PIN, 1);     // NSS_DAT�����

    Send_num = _size -1;                                                    // ���ͳ���
    SendAddress = _srcAddress +1;                                           // ����ָ��
    RecAddress = 0;                                                         // ����ָ���0

    SX1211_CFG_CS();                                                        // CFG����Ƭѡ
    EUSCI_A_SPI_enable(HAL_SX1211_SPI_BASE);                                // ����SPI����ʼ����
    EUSCI_A_SPI_clearInterruptFlag(HAL_SX1211_SPI_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT); // ����ձ�־λ
    EUSCI_A_SPI_enableInterrupt (HAL_SX1211_SPI_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);   // �������ж�

    UCA0TXBUF = *(uint8 *)_srcAddress;                                  // ��ʼ����
}


void SX1211_Send_Bytes_SPI (uint32 _srcAddress, uint16 _size)               // SX1211�����ֽڣ�SPIģʽ
{
    if (_size == 0)     return;                                             // �����ݣ��˳�

    EUSCI_A_SPI_disable(HAL_SX1211_SPI_BASE);                               // �ر�SPI
    SX1211_SPI_BUSY = TRUE;                                                 // SPIռ��
    SX1211_CFG_DE_CS ();                                                    // �������֣�CS����

    Send_num = _size -1;                                                    // ���ͳ���
    SendAddress = _srcAddress +1;                                           // ����ָ��
    RecAddress = 0;                                                         // ����ָ���0

    MCU_IO_PERIPHERAL_1 (HAL_SX1211_CS_DATA_PORT, HAL_SX1211_CS_DATA_PIN, MCU_IO_DIR_OUTPUT);   // CSʹ��
    EUSCI_A_SPI_enable(HAL_SX1211_SPI_BASE);                                // ����SPI����ʼ����
    EUSCI_A_SPI_clearInterruptFlag(HAL_SX1211_SPI_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);         // ����ձ�־λ
    EUSCI_A_SPI_enableInterrupt (HAL_SX1211_SPI_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);           // �������ж�

    UCA0TXBUF = *(uint8 *)_srcAddress;                                  // ��ʼ����
}


void SX1211_Read_CFG_SPI (uint32 _srcAddress, uint32 _recAddress, uint16 _size) // SX1211�������SPIģʽ
{
    if (_size == 0)     return;                                             // �����ݣ��˳�

    EUSCI_A_SPI_disable(HAL_SX1211_SPI_BASE);                               // �ر�SPI
    SX1211_SPI_BUSY = TRUE;                                                 // SPIռ��
    MCU_IO_OUTPUT (HAL_SX1211_CS_DATA_PORT, HAL_SX1211_CS_DATA_PIN, 1);     // NSS_DAT�����

    Send_num = _size -1;                                                    // ���ͳ���
    Rec_num = 0;                                                            // ���ճ���
    SendAddress = _srcAddress +1;                                           // ����ָ��
    RecAddress = _recAddress;                                               // ����ָ��

    SX1211_CFG_CS();                                                        // CFG����Ƭѡ
    EUSCI_A_SPI_enable(HAL_SX1211_SPI_BASE);                                // ����SPI����ʼ����
    EUSCI_A_SPI_clearInterruptFlag(HAL_SX1211_SPI_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT); // ����ձ�־λ
    EUSCI_A_SPI_enableInterrupt (HAL_SX1211_SPI_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);   // �������ж�

    UCA0TXBUF = *(uint8 *)_srcAddress;                                      // ��ʼ����
}


void SX1211_Read_Bytes_SPI (uint32 _srcAddress, uint32 _recAddress, uint16 _size)   // SX1211�����ֽڣ�SPIģʽ
{
    if (_size == 0)     return;                                             // �����ݣ��˳�

    EUSCI_A_SPI_disable(HAL_SX1211_SPI_BASE);                               // �ر�SPI
    SX1211_SPI_BUSY = TRUE;                                                 // SPIռ��
    SX1211_CFG_DE_CS ();                                                    // �������֣�CS����

    Send_num = _size -1;                                                    // ���ͳ���
    Rec_num = 0;                                                            // ���ճ���
    SendAddress = _srcAddress +1;                                           // ����ָ��
    RecAddress = _recAddress;                                               // ����ָ��

    MCU_IO_PERIPHERAL_1 (HAL_SX1211_CS_DATA_PORT, HAL_SX1211_CS_DATA_PIN, MCU_IO_DIR_OUTPUT);   // CSʹ��
    EUSCI_A_SPI_enable(HAL_SX1211_SPI_BASE);                                // ����SPI����ʼ����
    EUSCI_A_SPI_clearInterruptFlag(HAL_SX1211_SPI_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);     // ����ձ�־λ
    EUSCI_A_SPI_enableInterrupt (HAL_SX1211_SPI_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);       // �������ж�

    UCA0TXBUF = *(uint8 *)_srcAddress;                                  // ��ʼ����
}



HAL_ISR_FUNCTION(SX1211_ISR,USCI_A0)                                    // UCSI A0 �����ж�
{
    if (RecAddress == 0)                                                // ���Ϊ��������
    {
        Rec_num = UCA0RXBUF;                                            // ��ȡUCA0RXBUF�����жϱ�־
    }
    else                                                                // ���н�������
    {
        *(uint8 *)RecAddress = UCA0RXBUF;                               // �յ��������뻺��
        RecAddress++;                                                   // ��ַ++
        Rec_num++;                                                      // ����++
    }

    if (Send_num != 0)                                                  // δ�������
    {
        UCA0TXBUF = *(uint8 *)SendAddress;                              // ������һ��
        SendAddress++;                                                  // ��ַ++
        Send_num--;                                                     // ����--
    }
    else                                                                // �������
    {
        SX1211_CFG_DE_CS ();                                            // ����CS����
        EUSCI_A_SPI_disableInterrupt (HAL_SX1211_SPI_BASE, EUSCI_A_SPI_RECEIVE_INTERRUPT);  // �ؽ����ж�
        EUSCI_A_SPI_disable(HAL_SX1211_SPI_BASE);                       // �ر�SPI
        SX1211_SPI_BUSY = FALSE;                                        // ���SPIռ��
        __low_power_mode_off_on_exit();                                 // �˳�SPI������CPU
    }
}


void DMA0_isr (void)                                                    // DMA0�жϴ������
{   //  ����ǰһ�׶η������
    SX1211_CFG_DE_CS ();                                                // ����CS����
    DMA_disableInterrupt(DMA_CHANNEL_0);                                // �ر�DMA0�ж�
    TA2CTL = 0;

    SX1211_SPI_BUSY = FALSE;                                            // ���SPIռ��
    DMA_WAKE_INT = TRUE;                                                // �˳�DMA������CPU
}

