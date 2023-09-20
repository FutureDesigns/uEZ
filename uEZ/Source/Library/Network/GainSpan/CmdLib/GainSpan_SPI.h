/*-------------------------------------------------------------------------*
 * File:  GainSpan_SPI.h
 *-------------------------------------------------------------------------*
 * Description:
 *     GainSpan SPI Serial Driver.  Emulates the same API as a UART
 *     driver.  It has a FIFO buffer for sending and receiving.
 *     By making multiple calls to GainSpan_SPI_Update, the receive
 *     FIFO buffer will automatically be filled as data is available as
 *     well as data waiting to be sent can be sent.
 *-------------------------------------------------------------------------*/
#ifndef _GainSpan_SPI_H
#define _GainSpan_SPI_H

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "GainSpan_Config.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define GAINSPAN_SPI_CHAR_IDLE              0xF5
#define GAINSPAN_SPI_CHAR_ESC               0xFB
#define GAINSPAN_SPI_CHAR_FLOW_CONTROL_ON   0xFD
#define GAINSPAN_SPI_CHAR_FLOW_CONTROL_OFF  0xFA
#define GAINSPAN_SPI_CHAR_INACTIVE_LINK     0x00
#define GAINSPAN_SPI_CHAR_INACTIVE_LINK2    0xFF
#define GAINSPAN_SPI_CHAR_LINK_READY        0xF3

#define GAINSPAN_SPI_CHAR_UNKNOWN_F1		0xF1
#define GAINSPAN_SPI_CHAR_UNKNOWN_F4		0xF4
#define GAINSPAN_SPI_CHAR_UNKNOWN_F7		0xF7

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void GainSpan_SPI_Start(void);
void GainSpan_SPI_Stop(void);
bool GainSpan_SPI_ReceiveByte(uint8_t channel, uint8_t *aByte);
//bool GainSpan_SPI_SendByte(uint8_t aByte);
uint16_t GainSpan_SPI_SendData(const uint8_t *aData, uint16_t aLen);
void GainSpan_SPI_SendDataBlock(uint8_t channel, const uint8_t *aData, uint16_t aLen);
bool GainSpan_SPI_IsTransmitEmpty(void);
void GainSpan_SPI_Update(uint8_t channel);
bool GainSpan_SPI_SendByteLowLevel(uint8_t aByte);
bool GainSpan_SPI_IsLinkActive(void);
void GainSpan_SPI_FlowAllowed(void);

#endif // _GainSpan_SPI_H
/*-------------------------------------------------------------------------*
 * End of File:  GainSpan_SPI.h
 *-------------------------------------------------------------------------*/
