/*  ����� 2.3�档2017.6.12
    Ŀ�꣺�ھ����ܲ�Ӱ�칦�ĵ�ǰ���£�ʵ�����ݵ��շ�
    �������ʵ���ı���ASCII������������ʺ����ݵĴ����շ�
        ��������ݲ�������Ҫ�޸Ĵ���
    ��������δ��������У�飬�д������Ŀ���
    ��������������Χģ��״̬��ֻ�շ�����
    �������ѿ���Ƕ������
    �������֧��Ӣ���ַ�����ʾ����֧��ͼ�������ģ�
    ������ʹ�û���+DMA2+Ӳ��SPI��������
    �����򻺴�ǻ��ṹ�����������ȴ�ȫ��������Ϸ����ٴ�д�롣
    ����Ԫ������·���ƣ��������ʲ��ɸ���500K������

    ���� 0x10-0x1F Ϊָ���롣
    ���ն˽��յ���ʶ��Ĳ����룬����ս��ջ��沢��λ
    ����Ϊ�ַ������ն˲�����ʶ����ַ���>=0x80����

    ������֧������ָ�����ַ�����λ��ꡢ������LED��
        ��Ӧָ���룺0x11��0x12��0x13��0x14
    ֧�ֵ�ͨ�����%d:�з���int        %u:�޷���int       %x:16λ16��������        %l:32λ�з�������     %%
                \b  \n

    DMA����MCLKΪʱ��Դ�����SPIҲ��SPIΪʱ��Դ���ӿ��ٶ�

    ����������BUG���ڷ��ʹ���С���ݰ�ʱ���ܴ������
*/
/***********************************************************************************
* INCLUDES
*/

#include <diagnose.h>
#include <macros.h>
#include <stdarg.h>
#include <types.h>
#include <msp430.h>
#include <hal.h>
#include <dma.h>
#include <spi.h>

static bool diag_flag = TRUE;                                   // ���ģ���־ (��ʼʱռ�ã�����init_diag���ʹ�ܱ�ģ��)
static bool diag_sending = FALSE;                               // ��ʼδ����

uint8   Diag_BUF[HAL_BOARD_DIAG_BUF_LENGTH];                        // �������ݻ�����

static uint16   Diag_ptr_head, Diag_ptr_tail;                       // ��������βָ��


void init_diag (void)                                               // ��ϳ�ʼ�������ھ����ʼ������ã�����
{
//  ��ʼ��SPI A1Ϊ��ģʽ

    MCU_IO_PERIPHERAL_1 (HAL_BOARD_DIAG_SCK_PORT, HAL_BOARD_DIAG_SCK_PIN, MCU_IO_DIR_OUTPUT);   // UCA1 IO
    MCU_IO_PERIPHERAL_1 (HAL_BOARD_DIAG_STE_PORT, HAL_BOARD_DIAG_STE_PIN, MCU_IO_DIR_OUTPUT);   // UCA1 IO
    MCU_IO_PERIPHERAL_1 (HAL_BOARD_DIAG_MOSI_PORT, HAL_BOARD_DIAG_MOSI_PIN, MCU_IO_DIR_OUTPUT); // UCA1 IO
    MCU_IO_PERIPHERAL_1 (HAL_BOARD_DIAG_MISO_PORT, HAL_BOARD_DIAG_MISO_PIN, MCU_IO_DIR_OUTPUT); // UCA1 IO

    EUSCI_A_SPI_disable(HAL_BOARD_DIAG_BASE);                               // �ر�SPI

    EUSCI_A_SPI_masterInit(HAL_BOARD_DIAG_BASE,                             // UCA1
                           EUSCI_A_SPI_CLOCKSOURCE_SMCLK,                   // SMCLK
                           (uint32)HAL_CPU_CLOCK_KHz *1000,
                           250000,                                          // 250K
                           EUSCI_A_SPI_MSB_FIRST,
                           EUSCI_A_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,
                           EUSCI_A_SPI_CLOCKPOLARITY_INACTIVITY_LOW,
                           EUSCI_A_SPI_4PIN_UCxSTE_ACTIVE_HIGH              // 4�ߣ�CS����Ч
                           );

    EUSCI_A_SPI_select4PinFunctionality(HAL_BOARD_DIAG_BASE, EUSCI_A_SPI_ENABLE_SIGNAL_FOR_4WIRE_SLAVE);    // ʹ��CS����

//  ��ʼ��DMA��ͨ��2
        /*
         * Configure DMA channel 2
         * Configure channel for SINGLE transfers
         * DMA interrupt flag will be set after every 0 transfers
         * Use UCA1TXIFG to trigger transfers
         * Transfer BYTE-to-BYTE
         * Trigger upon Rising Edge of Trigger Source Signal
         */
    DMA_initialize(DMA_CHANNEL_2,
             DMA_TRANSFER_SINGLE,
             0,
             DMA2TSEL__UCA1TXIFG,
             DMA_SIZE_SRCBYTE_DSTBYTE,
             DMA_TRIGGER_RISINGEDGE);
        /*
         * DMA transfers inhibited during read-modify-write CPU operations
         */
    DMA_disableTransferDuringReadModifyWrite ();

        /*
         * Configure DMA channel 2
         * Use UCA1TXBUF as destination
         * destination address UNCHANGED
         */
    DMA_setDstAddress(DMA_CHANNEL_2,
                      (uint32)&UCA1TXBUF,
                      DMA_DIRECTION_UNCHANGED);


//  ��ʼ������
    Diag_ptr_head = Diag_ptr_tail = 0;                                      // ��β��ַ��ֵΪ0
    diag_flag = FALSE;                                                      // ����δ������
    diag_sending = FALSE;                                                   // ��ʼδ����
}



#pragma __printf_args
void diagnose (const uint8 *ptr,...)                            // ���������Ϣ
{
    if (diag_flag == TRUE)  return;                                         // �����ѱ����ã���ֹǶ�ף��˳�
    if (Diag_ptr_tail >= HAL_BOARD_DIAG_BUF_LENGTH)     return;             // ���������˳�
    diag_flag = TRUE;                                                       // �ñ�־λ


    Diag_BUF[Diag_ptr_tail] = 0x11;                                         // ָ��ַ�
    Diag_ptr_tail++;

//  %d      %u      %x      %%
    va_list app;
    va_start(app, ptr);                                                  // �䳤����

    while (*ptr != '\0')
    {
        if (Diag_ptr_tail >=HAL_BOARD_DIAG_BUF_LENGTH)  break;              // ������������

        Diag_BUF[Diag_ptr_tail] = *ptr;                                     // ֱ�����
        Diag_ptr_tail++;

                    if (*ptr == '%')                                        // ��Ϊ%
                    {
                        ptr++;
                        Diag_BUF[Diag_ptr_tail] = *ptr;                     // ֱ�����ͨ���
                        Diag_ptr_tail++;

                    /***********************    �з���int  ************************************************************/
                        if (*ptr == 'd')                                    // ������ͣ��з���ʮ��������(int)
                        {
                            uint16 app_value;
                            app_value = va_arg (app, uint16);               // ����

                            Diag_BUF[Diag_ptr_tail] = HI_UINT16 (app_value);// ��8λ
                            Diag_ptr_tail++;
                            Diag_BUF[Diag_ptr_tail] = LO_UINT16 (app_value);// ��8λ
                            Diag_ptr_tail++;
                        }

                    /***********************    �޷���int  ************************************************************/
                        else if (*ptr == 'u')                                   // ������ͣ��޷���ʮ��������(unsigned int)
                        {
                            uint16 app_value;
                            app_value = va_arg (app, uint16);               // ����

                            Diag_BUF[Diag_ptr_tail] = HI_UINT16 (app_value);// ��8λ
                            Diag_ptr_tail++;
                            Diag_BUF[Diag_ptr_tail] = LO_UINT16 (app_value);// ��8λ
                            Diag_ptr_tail++;
                        }

                    /***********************    16λ16��������   ************************************************************/
                        else if (*ptr == 'x')                                   // ������ͣ�ʹ���޷���ʮ����������
                        {
                            uint16 app_value;
                            app_value = va_arg (app, uint16);               // ����

                            Diag_BUF[Diag_ptr_tail] = HI_UINT16 (app_value);// ��8λ
                            Diag_ptr_tail++;
                            Diag_BUF[Diag_ptr_tail] = LO_UINT16 (app_value);// ��8λ
                            Diag_ptr_tail++;
                        }

                    /***********************    32λ�з�������    ************************************************************/
                        else if (*ptr == 'l')                                   // ������ͣ��з���ʮ��������(long)
                        {
                            uint32 app_value;
                            app_value = va_arg (app, uint32);               // ����

                            Diag_BUF[Diag_ptr_tail] = BREAK_UINT32 (app_value, 3);// 24-31λ
                            Diag_ptr_tail++;
                            Diag_BUF[Diag_ptr_tail] = BREAK_UINT32 (app_value, 2);// 16-23λ
                            Diag_ptr_tail++;
                            Diag_BUF[Diag_ptr_tail] = BREAK_UINT32 (app_value, 1);// 8-15λ
                            Diag_ptr_tail++;
                            Diag_BUF[Diag_ptr_tail] = BREAK_UINT32 (app_value, 0);// 0-7λ
                            Diag_ptr_tail++;
                        }
                    }

        ptr++;
    }

    diag_sendout ();                                                        // ���ݷ���

    diag_flag = FALSE;                                                      // ����������
}


void diag_cur (uint8 cur_y, uint8 cur_x)                        // ��ʾ�����趨
{
    if (diag_flag == TRUE)  return;                                         // �����ѱ����ã���ֹǶ�ף��˳�
    if (Diag_ptr_tail >= (HAL_BOARD_DIAG_BUF_LENGTH -3))    return;         // ����Ƿ��пռ�
    diag_flag = TRUE;                                                       // �ñ�־λ

    Diag_BUF[Diag_ptr_tail] = 0x12;                                         // ָ�����
    Diag_ptr_tail++;
    Diag_BUF[Diag_ptr_tail] = cur_y;                                        // Y ����
    Diag_ptr_tail++;
    Diag_BUF[Diag_ptr_tail] = cur_x;                                        // X ����
    Diag_ptr_tail++;

    diag_sendout ();                                                        // ���ݷ���
    diag_flag = FALSE;                                                      // ����������
}


void diag_clrscr (void)                                         // �������
{
    if (diag_flag == TRUE)  return;                                         // �����ѱ����ã���ֹǶ�ף��˳�
    if (Diag_ptr_tail >= (HAL_BOARD_DIAG_BUF_LENGTH -1))    return;         // ����Ƿ��пռ�
    diag_flag = TRUE;                                                       // �ñ�־λ

    Diag_BUF[Diag_ptr_tail] = 0x13;                                         // ָ�����
    Diag_ptr_tail++;

    diag_sendout ();                                                        // ���ݷ���
    diag_flag = FALSE;                                                      // ����������
}


void diag_power (uint16 _level)                                 // ������ʾ
{
    if (diag_flag == TRUE)  return;                                         // �����ѱ����ã���ֹǶ�ף��˳�
    if (Diag_ptr_tail >= (HAL_BOARD_DIAG_BUF_LENGTH -3))    return;         // ����Ƿ��пռ�
    diag_flag = TRUE;                                                       // �ñ�־λ

    _level &= 0x7FFF;                                                       // ���λΪLCD��RST������Ϊ1

    Diag_BUF[Diag_ptr_tail] = 0x14;                                         // ָ�����
    Diag_ptr_tail++;
    Diag_BUF[Diag_ptr_tail] = HI_UINT16 (_level);                           // ��8λ
    Diag_ptr_tail++;
    Diag_BUF[Diag_ptr_tail] = LO_UINT16 (_level);                           // ��8λ
    Diag_ptr_tail++;

    diag_sendout ();                                                        // ���ݷ���
    diag_flag = FALSE;                                                      // ����������
}


uint8 diag_busy (void)                                          // ���DIAG�Ƿ�æ        ������δ������
{
    if (diag_sending == TRUE)       return (TRUE);
    else    return (FALSE);
}


void diag_sendout (void)                                        // ���ͻ�������
{
    if (diag_sending == TRUE)   return;                                     // �����ڷ��ͣ�����

    while (EUSCI_A_SPI_isBusy (HAL_BOARD_DIAG_BASE) == EUSCI_A_SPI_BUSY);   // �ȴ�֮ǰ���һ�ֽڷ������


    EUSCI_A_SPI_disable(HAL_BOARD_DIAG_BASE);                               // �ر�SPI
    EUSCI_A_SPI_clearInterruptFlag(HAL_BOARD_DIAG_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);    // ���־λ

//  ��ʼ��������
    DMA_setSrcAddress(DMA_CHANNEL_2, ((uint32)&Diag_BUF) + Diag_ptr_head, DMA_DIRECTION_INCREMENT);    // ����DMA2ԭ��ַ
    DMA_setTransferSize(DMA_CHANNEL_2, (Diag_ptr_tail - Diag_ptr_head));    // ���ô���ߴ�
    DMA_clearInterrupt(DMA_CHANNEL_2);                                      // ���жϱ�־λ
    DMA_installInterrupt(DMA_CHANNEL_2, DMA2_diag_isr);                      // ����DMA2�ж�
    DMA_enableTransfers(DMA_CHANNEL_2);                                     // ʹ��DMA2

    Diag_ptr_head = Diag_ptr_tail;                                          // �����Ѵ�����ָ�����

    diag_sending = TRUE;                                                    // �����б�־        �������ڿ���SPI֮ǰ����
    EUSCI_A_SPI_enable(HAL_BOARD_DIAG_BASE);                                // ����SPI����ʼ����
}


void DMA2_diag_isr (void)                                       // DMA�����жϴ������
{
//  �׶��Է����Ѿ���ɣ�����Ƿ������ݴ�����

    if (Diag_ptr_head == Diag_ptr_tail)                                     // ������ȫ���������
    {
        DMA_disableInterrupt(DMA_CHANNEL_2);                                // �ر�DMA2�ж�
        Diag_ptr_head = Diag_ptr_tail = 0;                                  // ��β��ַ��0
        diag_sending = FALSE;                                               // �巢�ͱ�־
    }
    else
    {
        DMA_setSrcAddress(DMA_CHANNEL_2, ((uint32)&Diag_BUF) + Diag_ptr_head, DMA_DIRECTION_INCREMENT);     // ����DMA2ԭ��ַ
        DMA_setTransferSize(DMA_CHANNEL_2, (Diag_ptr_tail - Diag_ptr_head));    // ���ô���ߴ�
        Diag_ptr_head = Diag_ptr_tail;                                          // �����Ѵ�����ָ�����

        while ((EUSCI_A_SPI_getInterruptStatus(HAL_BOARD_DIAG_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT)) ==0);  // Ϊ����������ȴ���һ�η�����ϣ����ͻ���ճ���Ȼ�����´�����ʵ���ϵȴ�ʱ��Ϊ0��

        EUSCI_A_SPI_clearInterruptFlag(HAL_BOARD_DIAG_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);// ���־λ
        DMA_enableTransfers(DMA_CHANNEL_2);                                                 // ʹ��DMA2
        EUSCI_A_SPI_setInterruptFlag(HAL_BOARD_DIAG_BASE, EUSCI_A_SPI_TRANSMIT_INTERRUPT);  // �ñ�־λ�����´���
    }
}
