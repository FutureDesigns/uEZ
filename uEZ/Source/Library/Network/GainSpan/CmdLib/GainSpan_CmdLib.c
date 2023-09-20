/*-------------------------------------------------------------------------*
 * File:  GainSpan_CmdLib.c
 *-------------------------------------------------------------------------*/
/**
 *    @addtogroup GainSpan_CmdLib
 *  @{
 *  @brief     uEZ GainSpan command library
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    uEZ GainSpan command library
 *
 * @par Example code:
 * Example task to
 * @par
 * @code
 * #include <uEZ.h>
 * TODO
 * @endcode
 */
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h> /* for sprintf(), strstr(), strlen() , strtok() and strcpy()  */
//#include <stdlib.h> // causes re-defined errors in Crossworks
#include <ctype.h>
#include "GainSpan_Config.h"
#include "GainSpan_SPI.h"
#include "GainSpan_CmdLib.h"
#include <uEZSPI.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define GAINSPAN_AT_DEFAULT_TIMEOUT 2000

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static T_GainSpan_CmdLib_SPISettings G_GSCmdLibSettings;
static T_uezDevice G_GSSPI;
static SPI_Request G_GSSPIRequest;
static SPI_Request G_GSSPITemplate;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void IGainSpan_CmdLib_Write(const void *txData, uint16_t dataLength)
{
    const uint8_t *tx = (uint8_t *)txData;

#if 0
    while (dataLength--) {
        /* Keep trying to send this data until it goes */
        while (!GainSpan_SPI_SendByte(*tx)) {
            /* Process any incoming data as well */
            GainSpan_SPI_Update(GAINSPAN_SPI_CHANNEL);
        }

// Process any way
//GainSpan_SPI_Update(GAINSPAN_SPI_CHANNEL);

        tx++;
    }
#else
    while (dataLength) {
    	int32_t numSent = GainSpan_SPI_SendData(tx, dataLength);
    	dataLength -= numSent;
    	tx += numSent;
    	if (dataLength) {
    		UEZTaskDelay(1); // wait for remove
    		// TODO: Should wait some other way
    	}
    }
#endif
}

bool IGainSpan_CmdLib_Read(
    uint8_t *rxData,
    uint16_t dataLength,
    uint8_t blockFlag)
{
    bool got_data = false;

    /* Keep getting data if we have a number of bytes to fetch */
    while (dataLength) {
        /* Try to get a byte */
        if (GainSpan_SPI_ReceiveByte(GAINSPAN_SPI_CHANNEL, rxData)) {
//printf("|%02X|", *rxData);
            /* Got a byte, move up to the next position */
            rxData++;
            dataLength--;
            got_data = true;
        } else {
            /* Did not get a byte, are we block?  If not, stop here */
            if (!blockFlag)
                break;

            // TODO: Need some type of semaphore here!
        }
    }

    return got_data;
}

void GainSpan_CmdLib_Update(void)
{
    GainSpan_SPI_Update(GAINSPAN_SPI_CHANNEL);
}

void GainSpan_CmdLib_AllowFlow(void)
{
    GainSpan_SPI_FlowAllowed();
}

void IGainSpan_CmdLib_ProcessIncomingData(uint8_t rxData)
{
    // TODO: Hook back to the application?
    if (G_GSCmdLibSettings.iIncomingDataFunc)
        G_GSCmdLibSettings.iIncomingDataFunc(rxData);
}

void IGainSpan_CmdLib_EndIncomingData()
{
	//printf("End of Incoming data!!!\n");
    if (G_GSCmdLibSettings.iIncomingCompleteFunc)
        G_GSCmdLibSettings.iIncomingCompleteFunc();
}

bool GainSpan_IO_IsDataReady(uint8_t channel)
{
    return UEZGPIORead(G_GSCmdLibSettings.iDataReadyIO) ? true : false;
}

#if 0
bool GainSpan_SPI_Transfer(
    uint8_t channel,
    uint32_t numBytes,
    const uint8_t *send_buffer,
    uint8_t *receive_buffer,
    void (*callback)(void))
{
    T_uezError error;

    G_GSSPIRequest = G_GSSPITemplate;
    G_GSSPIRequest.iDataMOSI = send_buffer;
    G_GSSPIRequest.iDataMISO = receive_buffer;
    G_GSSPIRequest.iNumTransfers = numBytes;
    error = UEZSPITransferNoBlock(G_GSSPI, &G_GSSPIRequest,
        (T_spiCompleteCallback)callback, 0);
    if (error)
        return false;
    return true;
}
#else
bool GainSpan_SPI_Transfer(
    uint8_t channel,
    uint32_t numBytes,
    const uint8_t *send_buffer,
    uint8_t *receive_buffer,
    void (*callback)(void))
{
    T_uezError error;

    G_GSSPIRequest = G_GSSPITemplate;
    G_GSSPIRequest.iDataMOSI = send_buffer;
    G_GSSPIRequest.iDataMISO = receive_buffer;
    G_GSSPIRequest.iNumTransfers = numBytes;
    error = UEZSPITransferPolled(G_GSSPI, &G_GSSPIRequest);
    callback();
    if (error)
        return false;
    return true;
}
#endif

bool GainSpan_SPI_TransferPolled(
    uint8_t channel,
    uint32_t numBytes,
    const uint8_t *send_buffer,
    uint8_t *receive_buffer)
{
    T_uezError error;

    G_GSSPIRequest = G_GSSPITemplate;
    G_GSSPIRequest.iDataMOSI = send_buffer;
    G_GSSPIRequest.iDataMISO = receive_buffer;
    G_GSSPIRequest.iNumTransfers = numBytes;
    error = UEZSPITransferPolled(G_GSSPI, &G_GSSPIRequest);
    if (error)
        return false;
    return true;
}

bool GainSpan_SPI_IsBusy(uint8_t channel)
{
    return UEZSPIIsBusy(G_GSSPI);
}
/*---------------------------------------------------------------------------*
 * Routine:  GainSpan_CmdLib_ConfigureForSPI
 *--------------------------------------------------------------------------*/
/**   Configure SPI for GainSpan
 *
 *	@param [in] 	*aSettings	Pointer to settings structure
 *
 *	@return			T_uezError	UEZ_ERROR_NONE if success
 */
/*--------------------------------------------------------------------------*/
T_uezError GainSpan_CmdLib_ConfigureForSPI(
    const T_GainSpan_CmdLib_SPISettings *aSettings)
{
    T_uezError error;
    ATLIBGS_MSG_ID_E r;
    int32_t i;

    G_GSCmdLibSettings = *aSettings;
    G_GSSPITemplate.iBitsPerTransfer = 8;
    G_GSSPITemplate.iCSPolarity = EFalse; // Falling edge
    G_GSSPITemplate.iClockOutPolarity = EFalse; // Falling edge
    G_GSSPITemplate.iClockOutPhase = EFalse; // ETrue;
    G_GSSPITemplate.iDataMISO = 0;
    G_GSSPITemplate.iDataMISO = 0;
    G_GSSPITemplate.iNumTransferredIn = 0;
    G_GSSPITemplate.iNumTransferredOut = 0;
    G_GSSPITemplate.iRate = aSettings->iRate;
    G_GSSPITemplate.iCSGPIOPort = GPIO_TO_HAL_PORT(aSettings->iSPIChipSelect);
    G_GSSPITemplate.iCSGPIOBit = GPIO_TO_PIN_BIT(aSettings->iSPIChipSelect);
    G_GSSPITemplate.iFlags = SPI_REQUEST_TOGGLING_CS;

    error = UEZSPIOpen(G_GSCmdLibSettings.iSPIDeviceName, &G_GSSPI);
    if (error)
        return error;

    // Ensure these pins are not configured elsewhere
    UEZGPIOLock(G_GSCmdLibSettings.iDataReadyIO);

    if (G_GSCmdLibSettings.iSRSTn != GPIO_NONE ){
        UEZGPIOLock(G_GSCmdLibSettings.iSRSTn);
        UEZGPIOOutput(G_GSCmdLibSettings.iSRSTn);       // WIFI_SRSTn
    }

    UEZGPIOInput(G_GSCmdLibSettings.iDataReadyIO);       // WIFI IRQ

    if (G_GSCmdLibSettings.iProgramMode != GPIO_NONE ) {
        UEZGPIOLock(G_GSCmdLibSettings.iProgramMode);
        UEZGPIOClear(G_GSCmdLibSettings.iProgramMode);       // WIFI PROGRAM OFF
        UEZGPIOOutput(G_GSCmdLibSettings.iProgramMode);      // WIFI PROGRAM OFF
    }

    if (G_GSCmdLibSettings.iSPIMode != GPIO_NONE ) {
        UEZGPIOLock(G_GSCmdLibSettings.iSPIMode);
        UEZGPIOSet(G_GSCmdLibSettings.iSPIMode);          // WIFI_MODE SPI
        UEZGPIOOutput(G_GSCmdLibSettings.iSPIMode);       // WIFI_MODE SPI
    }

    UEZTaskDelay(1250);

    if (G_GSCmdLibSettings.iSRSTn != GPIO_NONE )
        UEZGPIOSet(G_GSCmdLibSettings.iSRSTn);          // WIFI_SRSTn

    UEZTaskDelay(1000);

    // Initialize the GainSpan SPI routines
    GainSpan_SPI_Start();

    // Flush anything there
    AtLibGs_FlushIncomingMessage();

    /* Send command to check 5 times for module */
    for (i = 0; i < 5; i++){
        AtLibGs_FlushIncomingMessage();
        r = AtLibGs_Check(GAINSPAN_AT_DEFAULT_TIMEOUT);
        if(ATLIBGS_MSG_ID_OK == r){
        break;
        }
    }
    if(ATLIBGS_MSG_ID_RESPONSE_TIMEOUT == r){
         return UEZ_ERROR_NOT_FOUND;
    }

    /* Send command to DISABLE echo */
    do {
        r = AtLibGs_SetEcho(ATLIBGS_DISABLE);
    } while (ATLIBGS_MSG_ID_OK != r);

    // For now, just report it is not found
    if (r != ATLIBGS_MSG_ID_OK)
        return UEZ_ERROR_NOT_FOUND;

    return UEZ_ERROR_NONE;
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  GainSpan_CmdLib.c
 *-------------------------------------------------------------------------*/
