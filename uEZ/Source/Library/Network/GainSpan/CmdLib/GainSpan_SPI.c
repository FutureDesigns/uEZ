/*-------------------------------------------------------------------------*
 * File:  GAINSPAN_SPI.c
 *-------------------------------------------------------------------------*/
/** @addtogroup GAINSPAN_SPI 
 * @{
 *     @brief GainSpan SPI Serial Driver.  Emulates the same API as a UART
 *     driver.  It has a FIFO buffer for sending and receiving.
 *     By making multiple calls to GainSpan_SPI_Update, the receive
 *     FIFO buffer will automatically be filled as data is available as
 *     well as data waiting to be sent can be sent.
 */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "GainSpan_Config.h"
#include "GainSpan_SPI.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#ifndef GAINSPAN_SPI_RX_BUFFER_SIZE
    #error "GAINSPAN_SPI_RX_BUFFER_SIZE must be defined in platform.h"
#endif
#ifndef GAINSPAN_SPI_TX_BUFFER_SIZE
    #error "GAINSPAN_SPI_TX_BUFFER_SIZE must be defined in platform.h"
#endif

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
/* Transmit FIFO buffer */
static uint8_t G_GainSpan_SPI_RXBuffer[GAINSPAN_SPI_RX_BUFFER_SIZE];
static uint16_t G_GainSpan_SPI_RXIn = 0;
static uint16_t G_GainSpan_SPI_RXOut = 0;

/* Transmit FIFO buffer */
static uint8_t G_GainSpan_SPI_TXBuffer[GAINSPAN_SPI_TX_BUFFER_SIZE];
static uint16_t G_GainSpan_SPI_TXIn = 0;
static uint16_t G_GainSpan_SPI_TXOut = 0;
static volatile bool G_GainSpan_SPI_TX_Empty;

static bool G_GainSpan_SPI_IsTransferComplete;
static bool G_GainSpan_SPI_IsTransferActive;
static uint16_t G_GainSpan_SPI_NumSent;
static bool G_GainSpan_SPI_EscapeCode;
static bool G_GainSpan_SPI_CanTransmit;
static bool G_GainSpan_SPI_IsLinkActive;
static uint8_t G_GainSpan_SPI_XONIdleChar;

/*---------------------------------------------------------------------------*
 * Routine:  GainSpan_SPI_Start
 *---------------------------------------------------------------------------*/
/** Initialize the GainSpan SPI FIFO driver on top of the RX62N's
 *      SPI RSPI0 driver.
 *      
 *  @return     void
 */
/*---------------------------------------------------------------------------*/
void GainSpan_SPI_Start(void)
{
    G_GainSpan_SPI_TXIn = G_GainSpan_SPI_TXOut = 0;
    G_GainSpan_SPI_RXIn = G_GainSpan_SPI_RXOut = 0;
    G_GainSpan_SPI_TX_Empty = true;
    G_GainSpan_SPI_EscapeCode = false;
    G_GainSpan_SPI_IsTransferComplete = false;
    G_GainSpan_SPI_IsTransferActive = false;
    G_GainSpan_SPI_NumSent = 0;
    G_GainSpan_SPI_CanTransmit = true;
    G_GainSpan_SPI_IsLinkActive = false;
}

/*---------------------------------------------------------------------------*
 * Routine:  GainSpan_SPI_Stop
 *---------------------------------------------------------------------------*/
/** Stop the GainSpan SPI driver.
 *      
 *  @return     void
 */
/*---------------------------------------------------------------------------*/
void GainSpan_SPI_Stop(void)
{
    /* Do nothing for now */
}

/*---------------------------------------------------------------------------*
 * Routine:  IGainSpan_SPI_TransferComplete
 *---------------------------------------------------------------------------*/
/** Interrupt Service Routine callback taht marks a transfer as
 *      completed.
 *      
 *  @return     void
 */
/*---------------------------------------------------------------------------*/
static void IGainSpan_SPI_TransferComplete(void)
{
    /* Note that the transfer is complete and processing needs to be done */
    G_GainSpan_SPI_IsTransferComplete = true;
}

/*---------------------------------------------------------------------------*
 * Routine:  GainSpan_SPI_ProcessIncomingChar
 *---------------------------------------------------------------------------*/
/** Checks for active link of data (ignores escape characters)
 *              If link is active and there is a char to store, then store
 *              character if there is room to store it.
 *      
 *  @param [in] c -- char to be processed
 *  @return     void
 */
/*---------------------------------------------------------------------------*/
static void GainSpan_SPI_ProcessIncomingChar(char c)
{
    bool storeChar = true;
    uint16_t next;

    /* Was the last character an escape code? */
    if (G_GainSpan_SPI_EscapeCode) {
        /* Use this character xor 0x20 and now no longer in escape mode */
        c ^= 0x20;
        G_GainSpan_SPI_EscapeCode = false;
    } else {
        // If we got a 0x00 or 0xFF without an escape, the SPI is not working right,
        // and is inactive.  Note it.
        if (c == GAINSPAN_SPI_CHAR_INACTIVE_LINK) {
            G_GainSpan_SPI_IsLinkActive = false;
            storeChar = false;
        } else if (c == GAINSPAN_SPI_CHAR_INACTIVE_LINK2) {
            G_GainSpan_SPI_IsLinkActive = false;
            storeChar = false;
        } else {
            // Got an active link worth of data
            G_GainSpan_SPI_IsLinkActive = true;
            if (c == GAINSPAN_SPI_CHAR_ESC) {
                /* Don't use this character, go into escape mode */
                G_GainSpan_SPI_EscapeCode = true;
                storeChar = false;
            } else if (c == GAINSPAN_SPI_CHAR_IDLE) {
                storeChar = false;
            } else if (c == GAINSPAN_SPI_CHAR_FLOW_CONTROL_ON) {
                G_GainSpan_SPI_CanTransmit = true;
                storeChar = false;
            } else if (c == GAINSPAN_SPI_CHAR_FLOW_CONTROL_OFF) {
                G_GainSpan_SPI_CanTransmit = false;
                storeChar = false;
            }
        }
    }
    if (storeChar) {
        /* The character needs to be stored in the receive buffer */
        /* Is there room? */
        next = G_GainSpan_SPI_RXIn + 1;
        if (next >= GAINSPAN_SPI_RX_BUFFER_SIZE)
            next = 0;
        if (next != G_GainSpan_SPI_RXOut) {
            /* Yes, room.  Store the character in the receive FIFO buffer */
//printf("{%02x,%d}", c, G_GainSpan_SPI_RXIn);
            G_GainSpan_SPI_RXBuffer[G_GainSpan_SPI_RXIn] = c;
            G_GainSpan_SPI_RXIn = next;
        } else {
//printf("\n\nError, out of space!\n\n");
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  GainSpan_SPI_ProcessIncoming
 *---------------------------------------------------------------------------*/
/** Process any incoming data bytes and put into the receive FIFO.
 *      Escape codes are also translated and IDLE characters are ignored.
 *      The number of bytes processed is based on G_GainSpan_SPI_NumSent
 *      and the bytes sitting in the FIFO going out (the outgoing bytes are
 *      replaced by the SPI low level routine with incoming bytes)
 *      
 *  @return     void
 */
/*---------------------------------------------------------------------------*/
static void GainSpan_SPI_ProcessIncoming(void)
{
    uint8_t c;
    uint16_t next;

    /* At this point, the characters in the transfer buffer */
    /* are characters that were sent and then replaced by the */
    /* matching received characters.  We need to process these */
    /* return characters and put the response in the receive */
    /* buffer. */
    /* Process all the bytes sent last */
    /* but only process if we have room in the receiving buffer */
    next = G_GainSpan_SPI_RXIn + 1;
    if (next >= GAINSPAN_SPI_RX_BUFFER_SIZE)
        next = 0;
    if (next != G_GainSpan_SPI_RXOut) {
        /* Was the last character sent used to watch for flow control? */
        if (!G_GainSpan_SPI_CanTransmit) {
            /* The returned character is now in the XON Idle character buffer */
            GainSpan_SPI_ProcessIncomingChar(G_GainSpan_SPI_XONIdleChar);
        } else {
            while (G_GainSpan_SPI_NumSent) {
                /* A character has come in, process it */
                /* The characters that are going out can be used */
                c = G_GainSpan_SPI_TXBuffer[G_GainSpan_SPI_TXOut++];
                if (G_GainSpan_SPI_TXOut == GAINSPAN_SPI_TX_BUFFER_SIZE)
                    G_GainSpan_SPI_TXOut = 0;
    
                next = G_GainSpan_SPI_RXIn + 1;
                if (next >= GAINSPAN_SPI_RX_BUFFER_SIZE)
                    next = 0;
                if (next != G_GainSpan_SPI_RXOut) {
                    GainSpan_SPI_ProcessIncomingChar(c);

                    G_GainSpan_SPI_NumSent--;
                } else {
                    break;
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  GainSpan_SPI_IsDataReady
 *---------------------------------------------------------------------------*/
/** Returns true of data is waiting, else returns false.
 *      
 *  @param [in] channel -- channel number
 *  @return     bool -- true if data is waiting to be pulled in over SPI, else false
 */
/*---------------------------------------------------------------------------*/
bool GainSpan_SPI_IsDataReady(uint8_t channel)
{
    return GainSpan_IO_IsDataReady(channel);
}

/*---------------------------------------------------------------------------*
 * Routine:  GainSpan_SPI_Update
 *---------------------------------------------------------------------------*/
/** Update the state of the GainSpan SPI driver.  It first determines
 *      if any data has just been received and if so, processes it.
 *      If SPI is not busy (and a transfer is no longer active), the
 *      outgoing FIFO is checked to see if bytes can be sent out over
 *      SPI.  If so, those bytes are scheduled to send out.
 *      If no bytes are to be sent, but the module has data to send us,
 *      we'll put an IDLE character in the FIFO and request for more
 *      to come in.
 *      
 *  @param [in] channel -- channel number
 *  @return     void
 */
/*---------------------------------------------------------------------------*/
void GainSpan_SPI_Update(uint8_t channel)
{
    uint16_t numBytes;

    /* Process any incoming bytes that were just sent */
    if (G_GainSpan_SPI_IsTransferComplete) {
        G_GainSpan_SPI_IsTransferComplete = false;
        GainSpan_SPI_ProcessIncoming();
        G_GainSpan_SPI_IsTransferActive = false;
    } else {
        /* Is the SPI bus busy? We cannot start a transfer until it is free. */
        if ((!GainSpan_SPI_IsBusy(channel)) && (!G_GainSpan_SPI_IsTransferActive)) {
            /* The SPI bus is now free to start another transfer */
            /* Try to send more data */
            /* Are we allowed to send data? (XON/XOFF) */
            if (G_GainSpan_SPI_CanTransmit) {
                /* Is there more data to send? */
                if (G_GainSpan_SPI_TXIn != G_GainSpan_SPI_TXOut) {
                    /* There is data to send, how many contiguous bytes can we send */
#if 0
                    if (G_GainSpan_SPI_TXIn > G_GainSpan_SPI_TXOut) {
                        numBytes = G_GainSpan_SPI_TXIn - G_GainSpan_SPI_TXOut;
                    } else {
                        numBytes = GAINSPAN_SPI_TX_BUFFER_SIZE
                                - G_GainSpan_SPI_TXOut;
                    }
#endif

                    // NOTE: Gainspan module wants only 1 byte per chip select cycle
                    numBytes = 1;

                    /* Remember how many bytes were sent in this transfer so */
                    /* the returned bytes can be processed later */
                    G_GainSpan_SPI_NumSent = numBytes;
                    G_GainSpan_SPI_IsTransferActive = true;

                    /* Tell the SPI to send out this group of characters */
                    GainSpan_SPI_Transfer(GAINSPAN_SPI_CHANNEL, numBytes,
                            G_GainSpan_SPI_TXBuffer + G_GainSpan_SPI_TXOut,
                            G_GainSpan_SPI_TXBuffer + G_GainSpan_SPI_TXOut,
                            IGainSpan_SPI_TransferComplete);
                    //MSTimerDelay(1);
                } else {
                    /* Nothing is being sent currently. */
                    /* Is the GainSpan module ready with data to return?  If so, send an IDLE character */
                    /* to start feeding out the data */
                    if (GainSpan_SPI_IsDataReady(GAINSPAN_SPI_CHANNEL)) {
                        GainSpan_SPI_SendByteLowLevel(GAINSPAN_SPI_CHAR_IDLE);
                    }
                }
            } else {
                /* XON is active.  We can send IDLE characters when data is ready */
                if (GainSpan_SPI_IsDataReady(GAINSPAN_SPI_CHANNEL)) {
                    // Send an IDLE character to look for the XON/XOFF */
                    G_GainSpan_SPI_XONIdleChar = GAINSPAN_SPI_CHAR_IDLE;
                    // No bytes from the buffer are being sent
                    G_GainSpan_SPI_NumSent = 0;
                    G_GainSpan_SPI_IsTransferActive = true;
                    GainSpan_SPI_Transfer(GAINSPAN_SPI_CHANNEL, 1,
                            &G_GainSpan_SPI_XONIdleChar,
                            &G_GainSpan_SPI_XONIdleChar,
                            IGainSpan_SPI_TransferComplete);
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  GainSpan_SPI_IsTransmitEmpty
 *---------------------------------------------------------------------------*/
/** Determine if there is any data waiting in the outgoing FIFO.
 *      
 *  @return     bool -- true if data is waiting to be sent, else false.
 */
/*---------------------------------------------------------------------------*/
bool GainSpan_SPI_IsTransmitEmpty(void)
{
    /* Return true if the transmit FIFO is empty (no data in or out) */
    return (G_GainSpan_SPI_TXOut == G_GainSpan_SPI_TXIn);
}

/*---------------------------------------------------------------------------*
 * Routine:  GainSpan_SPI_ReceiveByte
 *---------------------------------------------------------------------------*/
/** Get a byte in the GainSpan_SPI receive FIFO buffer.
 *      
 *  @param [in] channel -- channel number
 *  @param [in] aByte -- Returned byte (if any)
 *  @return     bool -- true if byte returned, else false
 */
/*---------------------------------------------------------------------------*/
bool GainSpan_SPI_ReceiveByte(uint8_t channel, uint8_t *aByte)
{
    bool found = false;

    /* When looking for a byte, update the state */
    GainSpan_SPI_Update(channel);

    /* Check to see if any bytes have been placed in the FIFO and */
    /* are waiting to be pulled out. */
    if (G_GainSpan_SPI_RXIn != G_GainSpan_SPI_RXOut) {
//printf("/%02x,%d/", G_GainSpan_SPI_RXBuffer[G_GainSpan_SPI_RXOut], G_GainSpan_SPI_RXOut);
        *aByte = G_GainSpan_SPI_RXBuffer[G_GainSpan_SPI_RXOut++];
#if 0
if (*aByte != 0xF5) {
    if ((*aByte >= 0x20) && (*aByte <= 0x7F))
        printf("<%c", *aByte);
    else
        printf("[<%02X]", *aByte);
}
#endif

        if (G_GainSpan_SPI_RXOut >= GAINSPAN_SPI_RX_BUFFER_SIZE)
            G_GainSpan_SPI_RXOut = 0;
        found = true;
    }

    return found;
}

/*---------------------------------------------------------------------------*
 * Routine:  GainSpan_SPI_SendByteLowLevel
 *---------------------------------------------------------------------------*/
/** Send a raw low level byte out the GainSpan SPI transmit FIFO.  This
 *      version of the routine does no escape characters or translating.
 *      
 *  @param [in] aByte -- Byte to send
 *  @return     bool -- true if byte was successfully placed in transmit FIFO, else
 *          false.
 */
/*---------------------------------------------------------------------------*/
bool GainSpan_SPI_SendByteLowLevel(uint8_t aByte)
{
    uint16_t next;
    bool placed = false;

#if 0
if (aByte != 0xF5) {
    if ((aByte >= 0x20) && (aByte <= 0x7F))
        printf(">%c", aByte);
    else
        printf("[>%02X]", aByte);
}
#endif

    /* Calculate where the next in position is in the FIFO */
    next = G_GainSpan_SPI_TXIn + 1;
    if (next >= GAINSPAN_SPI_TX_BUFFER_SIZE)
        next = 0;

    /* Is there room in the transmit FIFO? */
    if (next != G_GainSpan_SPI_TXOut) {
        /* There is room, place a byte in the FIFO */
        G_GainSpan_SPI_TXBuffer[G_GainSpan_SPI_TXIn] = aByte;
        G_GainSpan_SPI_TXIn = next;
        placed = true;
    } else {
        /* There is no room in the FIFO, return the failed case */
        placed = false;
    }

    return placed;
}

/*---------------------------------------------------------------------------*
 * Routine:  GainSpan_SPI_SendByte
 *---------------------------------------------------------------------------*/
/** Send a byte out the transmit FIFO of the GainSpan_SPI driver.  If
 *      the byte needs to be encoded, this routine does the translation.
 *      Returns true if the byte is successfully encoded and sent,
 *      else returns false.
 *      
 *  @param [in] aByte -- Byte to send
 *  @return     bool -- true if byte was successfully placed in transmit FIFO, else
 *          false.
 */
/*---------------------------------------------------------------------------*/
bool GainSpan_SPI_SendByte(uint8_t aByte)
{
    uint16_t next;
    bool placed = false;

    /* If one of the special characters, stuff an extra byte into it */
    switch (aByte) {
        case GAINSPAN_SPI_CHAR_IDLE:
        case GAINSPAN_SPI_CHAR_ESC:
        case GAINSPAN_SPI_CHAR_FLOW_CONTROL_ON:
        case GAINSPAN_SPI_CHAR_FLOW_CONTROL_OFF:
        case GAINSPAN_SPI_CHAR_INACTIVE_LINK:
        case GAINSPAN_SPI_CHAR_INACTIVE_LINK2:
        case GAINSPAN_SPI_CHAR_LINK_READY:
            /* Is there room for two characters? */
            next = G_GainSpan_SPI_TXIn + 1;
            if (next >= GAINSPAN_SPI_TX_BUFFER_SIZE)
                next = 0;
            if (next != G_GainSpan_SPI_TXOut) {
                next++;
                if (next >= GAINSPAN_SPI_TX_BUFFER_SIZE)
                    next = 0;

                /* There is room for two bytes, now stuff the characters in */
                GainSpan_SPI_SendByteLowLevel(GAINSPAN_SPI_CHAR_ESC);
                placed = GainSpan_SPI_SendByteLowLevel(aByte ^ 0x20);
            }
            break;
        default:
            /*  Not a special character, go ahead and store it */
            placed = GainSpan_SPI_SendByteLowLevel(aByte);
            break;
    }

    return placed;
}

/*---------------------------------------------------------------------------*
 * Routine:  GainSpan_SPI_SendData
 *---------------------------------------------------------------------------*/
/** Send an array of data bytes out the transmit FIFO.  This routine
 *      does not block and returns the number of bytes sent.
 *      
 *  @param [in] aData -- data to send
 *  @param [in] aLen -- Number of bytes to send.
 *  @return     uint16_t -- Number of bytes actually buffered and sent.
 */
/*---------------------------------------------------------------------------*/
uint16_t GainSpan_SPI_SendData(const uint8_t *aData, uint16_t aLen)
{
    uint16_t i;

    /* Send each byte one at a time unless out of space */
    for (i = 0; i < aLen; ++i) {
        if (!GainSpan_SPI_SendByte(aData[i]))
            break;
    }

    /* Return the number of bytes that did get into the transmit FIFO */
    return i;
}

/*---------------------------------------------------------------------------*
 * Routine:  GainSpan_SPI_SendDataBlock
 *---------------------------------------------------------------------------*/
/** Send an array of data out the transmit FIFO.  This routine will
 *      block until all the bytes are sent.
 *      
 *  @param [in] channel -- channel
 *  @param [in] aData -- data to send
 *  @param [in] aLen -- Number of bytes to send.
 *  @return     void
 */
/*---------------------------------------------------------------------------*/
void GainSpan_SPI_SendDataBlock(uint8_t channel, const uint8_t *aData, uint16_t aLen)
{
    /* Send each byte one at a time unless out of space */
    uint16_t i;

    for (i = 0; i < aLen; ++i) {
        while (1) {
            if (GainSpan_SPI_SendByte(aData[i]))
                break;
            GainSpan_SPI_Update(channel);
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  GainSpan_SPI_IsLinkActive
 *---------------------------------------------------------------------------*/
/** Determine if the SPI connection is to an active module.  If it is,
 *      this returns true, else false.
 *      NOTE: Link active state changes based on received SPI bytes and
 *          may not immediately reflect the actual state -- particularly
 *          at startup or immediately after sending out characters.
 *      
 *  @return     bool    -- true if link is active, else false.
 */
/*---------------------------------------------------------------------------*/
bool GainSpan_SPI_IsLinkActive(void)
{
    return G_GainSpan_SPI_IsLinkActive;
}

/** @} */
/*-------------------------------------------------------------------------*
 * File:  GAINSPAN_SPI.h
 *-------------------------------------------------------------------------*/
