//#include "../glyph_cfg.h"
#include "iodefine.h"
#include <Device/SPIBus.h>
#include <Source/Library/Graphics/Glyph/glyph_api.h>
#include <Source/Library/Graphics/Glyph/glyph_cfg.h>
#include <Source/Library/Graphics/Glyph/glyph_types.h>

/******************************************************************************
Private Functions
******************************************************************************/
static DEVICE_SPI_BUS **G_spi = 0;
static SPI_Request G_request;

/******************************************************************************
* ID : 60.0
* Outline : Glyph_uEZ_Open
* Include : Glyph_uEZ_0.h
* Function Name: Glyph_uEZ_Open
* Description : Open and setup the communications channel 0.
* Argument : aHandle - the Glyph handle to setup for the LCD and Communications.
* Return Value : 0=success, not 0= error
* Calling Functions : GlyphCommOpen
******************************************************************************/
T_glyphError Glyph_uEZ_Open(T_glyphHandle aHandle)
{
    T_uezDevice dev;
    T_uezDeviceWorkspace *G_gpioC;
    
    if (G_spi == 0)  {
        UEZDeviceTableFind("SPI0", &dev);
        UEZDeviceTableGetWorkspace(dev, (T_uezDeviceWorkspace **)&G_spi);
        HALInterfaceFind("GPIOC", (T_halWorkspace **)&G_gpioC);

        G_request.iBitsPerTransfer = 8;
        // Chip select on Port C2 (high true)
        G_request.iCSGPIOPort = (HAL_GPIOPort **)G_gpioC;
        G_request.iCSGPIOBit = (1<<2);
        G_request.iCSPolarity = EFalse;
        G_request.iClockOutPhase = EFalse;
        G_request.iClockOutPolarity = EFalse;
        G_request.iDataMISO = 0;
        G_request.iDataMOSI = 0;
        G_request.iNumTransfers = 0;
        G_request.iRate = 12000; // 12 MHz for now

        /* Set data direction for this bit to output */
		PORTC.PDR.BIT.B3 = 1; // RESET_IO
		PORT5.PDR.BIT.B1 = 1; // LCD-RS
    }

    return GLYPH_ERROR_NONE ;
}

/******************************************************************************
* ID : 65.0
* Outline : Glyph_uEZ_TransferOutWord8Bit
* Include : Glyph_uEZ_0.h
* Function Name: Glyph_uEZ_TransferOutWord8Bit
* Description : Transmit out, the  Port 0, 8 Bits of data.
* Argument : cData - 8 bits to send
* Return Value : none
* Calling Functions : T_Comm_API.iWrite
******************************************************************************/
void Glyph_uEZ_TransferOutWord8Bit(uint8_t cData)
{
    //Glyph_uEZ_TransferOutWords(0, cData) ;
    G_request.iDataMOSI = &cData;
    G_request.iNumTransfers = 1;
    G_request.iDataMISO = 0;
    (*G_spi)->TransferPolled(G_spi, &G_request);
}

/******************************************************************************
* ID : 66.0
* Outline : SetSendCommand
* Include : Glyph_uEZ_0.h
* Function Name: SetSendCommand
* Description : Set the RS line of the LCD.  The RS line is connected to 
* output port P5.1.  Setting this output to 0 will allow data flow in the LCD.
* Argument : none
* Return Value : none
* Calling Functions : CommandSend, DataSend
******************************************************************************/
static void SetSendCommand(void)
{
    PORT5.PODR.BIT.B1 = 0; // LCD-RS
}

/******************************************************************************
* ID : 67.0
* Outline : SetSendData
* Include : Glyph_uEZ_0.h
* Function Name: SetSendData
* Description : Clear the RS line of the LCD.  The RS line is connected to
* output port P5.1.  Setting this output to 1 will stop data flow in the LCD.
* When data flow is stopped in the LCD the data is internally copied to the
* data buffer for use.
* Argument : none
* Return Value : none
* Calling Functions : CommandSend, DataSend
******************************************************************************/
static void SetSendData(void)
{
    PORT5.PODR.BIT.B1 = 1; // LCD-RS
}

/******************************************************************************
* ID : 68.0
* Outline : StartResetLCD
* Include : Glyph_uEZ_0.h
* Function Name: StartResetLCD
* Description : Set the A0 line of the LCD display.  This line is connected
* to output PC.3 of the RX62N.  This output is inversed.  Setting this output
* to 1 will cause the LCD to reset.
* Argument : none
* Return Value : none
* Calling Functions : ST7579_Config
******************************************************************************/
/*static void StartResetLCD(void)
{
    PORTC.PODR.BIT.B3 = 1; // RESET_IO
}*/

/******************************************************************************
* ID : 69.0
* Outline : EndResetLCD
* Include : Glyph_uEZ_0.h
* Function Name: EndResetLCD
* Description : Clear the A0 line of the LCD display.  This line is connected
* to output PC.3 of the RX62N.  This output is inversed.  Setting this output
* to 0 will cause the LCD complete reset.  After a delay the LCD will be
* ready to utilize.
* Argument : none
* Return Value : none
* Calling Functions : ST7579_Config
******************************************************************************/
/*static void EndResetLCD(void)
{
    PORTC.PODR.BIT.B3 = 0; // RESET_IO
}*/

/******************************************************************************
* ID : 70.0
* Outline : Glyph_uEZ_CommandSend
* Include : Glyph_uEZ_0.h
* Function Name: Glyph_uEZ_CommandSend
* Description : Send a command define to the LCD.  The defines for all commands
* available to send to the ST7579 LCD controller are defined in the header
* for this file RX62N_LCD.h.  The nCommand parameter should always be
* a define from that location.
* Argument : cCommand - 8 bits of data to be used as a command to the LCD
*          : aHandle - the Glyph handle setup by the LCD and Communications.
* Return Value : none
* Calling Functions : ST7579_Config, ST7579_SetSystemBiasBooster,
*                     ST7579_SetVO_Range, ST7579_SetFrameRate,
*                     ST7579_SetPage, ST7579_SetChar, ST7579_SetLine,
*                     ST7579_Send8bitsData, ST7579_Send16bitsCommand
******************************************************************************/
void Glyph_uEZ_CommandSend(int8_t cCommand)
{
    /* Send Command To LCD */
    /* A0 = 0 */
    SetSendCommand() ;
    
    Glyph_uEZ_TransferOutWord8Bit(cCommand) ;

    /* A0 = 1 */
    SetSendData() ;
}

/******************************************************************************
* ID : 71.0
* Outline : Glyph_uEZ_DataSend
* Include : Glyph_uEZ_0.h
* Function Name: Glyph_uEZ_DataSend
* Description : Send 8 bits of data to the LCD RAM at the current location.
* The current location is determined and set with local functions.
* Argument : cCommand - 8 bits of data to be sent to the LCD
*          : aHandle - the Glyph handle setup by the LCD and Communications.
* Return Value : none
* Calling Functions : ST7579_Config, ST7579_SetSystemBiasBooster,
*                     ST7579_SetVO_Range, ST7579_SetFrameRate,
*                     ST7579_SetPage, ST7579_SetChar, ST7579_SetLine,
*                     ST7579_Send8bitsData, ST7579_Send16bitsCommand
******************************************************************************/
void Glyph_uEZ_DataSend(int8_t cData)
{
    /* A0 = 1 */
    SetSendData() ;

    Glyph_uEZ_TransferOutWord8Bit(cData) ;

    /* A0 = 1 */
    SetSendData() ;
}

