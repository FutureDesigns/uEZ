/**
 *  @file   GainSpan_Config.h
 *  @brief  uEZ GainSpan Config
 *
 *  This file is referenced by the drivers for necessary compile time
 *     options.  For most users, another .h file holds the compile
 *     time information and settings (e.g., YRDKRX63N.h)
 */
#ifndef GainSpan_Config_H_
#define GainSpan_Config_H_

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZRTOS.h>
#include <stdint.h>
#include <stdbool.h>


#ifndef ATLIBGS_TX_CMD_MAX_SIZE
#define ATLIBGS_TX_CMD_MAX_SIZE         (1024)
#endif

#ifndef ATLIBGS_RX_CMD_MAX_SIZE
#define ATLIBGS_RX_CMD_MAX_SIZE         (1536)
#endif
#ifndef GAINSPAN_SPI_RX_BUFFER_SIZE
#define GAINSPAN_SPI_RX_BUFFER_SIZE     (1024)
#endif
#ifndef GAINSPAN_SPI_TX_BUFFER_SIZE
#define GAINSPAN_SPI_TX_BUFFER_SIZE     (1024)
#endif

#define GAINSPAN_SPI_CHANNEL            0

// Error Code LED
//#define ERROR_LED_ON()   PORTE.PODR.BIT.B1 = 1;
//#define ERROR_LED_OFF()  PORTE.PODR.BIT.B1 = 0;

// Console (UART0) driver linkage
//#define Console_UART_Start(baud)              UART_SCI2_Start(baud)
//#define Console_UART_Stop()                   UART_SCI2_Stop()
//#define Console_UART_SendByte(aByte)          UART_SCI2_SendByte(aByte)
//#define Console_UART_SendData(aData, aLen)    UART_SCI2_SendData(aData, aLen)
//#define Console_UART_ReceiveByte(aByte)       UART_SCI2_ReceiveByte(aByte)
//#define Console_UART_IsTransmitEmpty()        UART_SCI2_IsTransmitEmpty()

// Application Header (UART2) driver linkage
//#define GainSpan_UART_Start(baud)             UART_SCI6_Start(baud)
//#define GainSpan_UART_Stop()                  UART_SCI6_Stop()
//#define GainSpan_UART_SendByte(aByte)         UART_SCI6_SendByte(aByte)
//#define GainSpan_UART_SendData(aData, aLen)   UART_SCI6_SendData(aData, aLen)
//#define GainSpan_UART_ReceiveByte(aByte)      UART_SCI6_ReceiveByte(aByte)
//#define GainSpan_UART_IsTransmitEmpty()       UART_SCI6_IsTransmitEmpty()

//#define NV_Open                               Flash_Open
//#define NV_Erase                              Flash_Erase
//#define NV_ChipErase                          Flash_ChipErase
//#define NV_Write                              Flash_Write
//#define NV_Read                               Flash_Read

#define MSTimerGet()                    UEZTickCounterGet()
#define MSTimerDelta(ms)                UEZTickCounterGetDelta(ms)
#define MSTimerDelay(ms)                UEZTaskDelay(ms)

#define _F8_                            "%d"
#define _F16_                           "%d"
#define _F32_                           "%d"

/**
 *	Write data to the GainSpan module
 *
 *	@param [in] 	*txData 	pointer to data to write
 *                          
 *	@param [in]		dataLength	Lenght of data to write
 *
 */
void IGainSpan_CmdLib_Write(const void *txData, uint16_t dataLength);
/**
 *	read data from the GainSpan module
 *
 *	@param [out	] 	*rxData		Pointer of area to store read data
 *                          
 *	@param [in]		dataLength	length of data to read
 *
 *	@param [in]		blockFlag	flag to block on
 *
 *	@return			bool	
 */
bool IGainSpan_CmdLib_Read(uint8_t *rxData, uint16_t dataLength, uint8_t blockFlag);
/**
 *	Update command status
 *
 */
void GainSpan_CmdLib_Update(void);
/**
 *	Process incoming data
 *
 *	@param [in] 	rxData	character to process
 *
 */
void IGainSpan_CmdLib_ProcessIncomingData(uint8_t rxData);

// Port the application calls in the AtCmdLibrary to our UEZ definitions
#define App_Write(txData, dataLength)   IGainSpan_CmdLib_Write(txData, dataLength)
#define App_Read(rxData, len, block)    IGainSpan_CmdLib_Read(rxData, len, block)
#define App_Update()                    GainSpan_CmdLib_Update()
#define App_ProcessIncomingData(c)      IGainSpan_CmdLib_ProcessIncomingData(c)
#define App_DelayMS(ms)                 UEZTaskDelay(ms)

// Outside hooks:
/**
 *	Check if data is ready
 *
 *	@param [in] 	channel channel to check
 *
 *	@return			bool	true if ready
 */
bool GainSpan_IO_IsDataReady(uint8_t channel);
/**
 *	Put the GainSpan module into programming mode
 */
void GainSpan_IO_ProgMode(void);
/**
 *	Transfer data to GainSpan over SPI
 *
 *	@param [in] 	channel 		Channel number
 *                          
 *	@param [in]		numBytes		Number of Bytes of send
 *
 *	@param [in] 	*send_buffer	Pointer to buffer of bytes to send
 *                          
 *	@param [out]	*receive_buffer	Pointer to buffer of bytes to receive
 *
 *	@param [in] 	callback 		Function pointer
 *
 *	@return			bool			true if success
 */
bool GainSpan_SPI_Transfer(
        uint8_t channel,
        uint32_t numBytes,
        const uint8_t *send_buffer,
        uint8_t *receive_buffer,
        void(*callback)(void));
/**
 *	Check if SPI is busy
 *
 *	@param [in] 	channel
 *                         
 *	@return			bool true if SPI is busy
 */
bool GainSpan_SPI_IsBusy(uint8_t channel);


#endif // GainSpan_Config_H_
/*-------------------------------------------------------------------------*
 * End of File:  GainSpan_Config.h
 *-------------------------------------------------------------------------*/
