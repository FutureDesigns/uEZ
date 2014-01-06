/*-------------------------------------------------------------------------*
 * File:  LPC1756_USBDeviceController.c
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
/*
 *  @{
 *  @brief     uEZ USB Device Controller Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZLicense.txt
 *
 *    HAL implementation of the LPC1756 USB Device Driver.
 *
 * @par Example code:
 * Example task to 
 * @par
 * @code
 * #include <uEZ.h>
 * TODO
 * @endcode
 */
#include <Config.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <HAL/Interrupt.h>
// TBD:  not here!
#include <uEZRTOS.h>
#include <Source/Processor/NXP/LPC1756/LPC1756_USBDeviceController.h>

/**------------------------------------------------------------------------*
 * USB Registers:
 *-------------------------------------------------------------------------*/

#define dprintf(...)

    /** USB Device Interrupts Bits: */
        /** every 1 ms interrupt (for isochronous packet transfers) */
        #define USB_DEV_FRAME                       (1<<0)

        /** Fast interrupt transfer */
        #define EP_FAST                     (1<<1)

        /** Slow interrupt tranfer */
        #define EP_SLOW                     (1<<2)

        /** Device status changed */
        #define DEV_STAT                    (1<<3)

        /** Command code register is empty (new commands can be written) */
        #define CCEMTY                      (1<<4)

        /** Command data register is full (data can be read now) */
        #define CDFULL                      (1<<5)

        /** Current packet in FIFO has been transferred to the CPU */
        #define RxENDPKT                    (1<<6)

        /** The number of data bytes transferred to the FIFO equals
         * the number of bytes programmed in the TxPacket length register */
        #define TxENDPKT                    (1<<7)

        /** Endpoints realized. */
        #define EP_RLZED                    (1<<8)

        /** Error interrupt. */
        #define ERR_INT                     (1<<9)


/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
/** Device commands: */
#define USB_DEVICE_CMD_SET_ADDRESS                      0xD0
    #define USB_DEVICE_SET_ADDRESS_ADDRESS		    (1<<0) /** 6 bits */
    #define USB_DEVICE_SET_ADDRESS_ENABLE		    (1<<7)
#define USB_DEVICE_CMD_CONFIG                           0xD8
    #define USB_DEVICE_CONFIG                               (1<<0)
#define USB_DEVICE_CMD_SET_MODE                         0xF3
#define USB_DEVICE_CMD_READ_CUR_USB_DEV_FRAME_NR        0xF5
#define USB_DEVICE_CMD_READ_TEST_REG                    0xFD
#define USB_DEVICE_CMD_STATUS                           0xFE
#define USB_DEVICE_CMD_GET_ERROR_CODE                   0xFF
#define USB_DEVICE_CMD_READ_ERROR_STATUS                0xFB

/** Endpoint commands: */
#define USB_ENDPOINT_CMD_SELECT                         0x00
#define USB_ENDPOINT_CMD_SELECT_CLEAR                   0x40
#define USB_ENDPOINT_CMD_SET_STATUS                     0x40
    #define ENDPOINT_STATUS_STALLED                         (1<<0)
    #define ENDPOINT_STATUS_DISABLED                        (1<<5)
    #define ENDPOINT_STATUS_RATE_FEEDBACK_MODE              (1<<6)
    #define ENDPOINT_STATUS_CONDITIONAL_STALL               (1<<7)
#define USB_ENDPOINT_CMD_CLEAR_BUFFER                   0xF2
#define USB_ENDPOINT_CMD_VALIDATE_BUFFER                0xFA

/** Bits: USB->USBCtrl */
#define RD_EN                   (1<<0)
#define WR_EN                   (1<<1)
#define LOG_ENDPOINT            (1<<2)

/** Bits: USB->USBRxPLen */
#define PKT_LNGTH               (1<<0) /** 10 bits */
    #define PKT_LNGTH_MASK          ((1<<10)-1)
#define DV                      (1<<10)
#define PKT_RDY                 (1<<11)


/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const HAL_USBDeviceController *iHAL;
    T_USBDeviceControllerSettings iSettings;
    TUInt8 iConfigurationNumber;

} T_LPC1756_USBDev_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
extern const HAL_USBDeviceController LPC1756_USBDev_Interface;
T_LPC1756_USBDev_Workspace *G_LPC1756_USBDev_Workspace=0;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static void LPC1756_USBDev_EndpointConfigure(
        void *aWorkspace,
        TUInt8 aEndpoint,
        TUInt16 aMaxPacketSize);

/*-------------------------------------------------------------------------*
 * Internal Function:  IEndpointToIndex
 *---------------------------------------------------------------------------*/
/**
 *  Convert a standard endpoint identifier to a LPC21xx USB endpoint
 *      index number.
 *
 *  @param [in]    aEndpoint   	Endpoint number
 *
 *  @return        TUInt32    	USB Hardware endpoint number
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static INLINE TUInt32 IEndpointToIndex(TUInt8 aEndpoint)
{
    TUInt32 index = aEndpoint & 0x0F;
    index <<= 1;
    index |= ((aEndpoint & 0x80)>>7);
    return index;
}

/*-------------------------------------------------------------------------*
 * Internal Function:  IIndexToEndpoint
 *---------------------------------------------------------------------------*/
/**
 *  Convert a LPC21xx USB endpoint index number to a standard
 *      endpoint identifier.
 *
 *  @param [in]    aIndex     	USB Hardware endpoint number
 *
 *  @return        TUInt8    	Endpoint number
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static INLINE TUInt8 IIndexToEndpoint(TUInt32 aIndex)
{
    TUInt8 endpointNum = (TUInt8) (aIndex>>1);
    endpointNum |= (aIndex & 1) << 7;
    return endpointNum;
}

/*-------------------------------------------------------------------------*
 * Internal Function:  IWaitForDeviceInterrupt
 *---------------------------------------------------------------------------*/
/**
 *  Wait for any of the given device interrupt flags to go from 1 to 0.
 *
 *  @param [in]    *p 					workspace
 *
 *  @param [in]    aDevInterruptFlags 	Flag(s) to wait to go from 0 to 1
 *
 *  @return        
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void IWaitForDeviceInterrupt(
                T_LPC1756_USBDev_Workspace *p,
                TUInt32 aDevInterruptFlags)
{
    PARAM_NOT_USED(p);
    while ((USB->USBDevIntSt & aDevInterruptFlags) != aDevInterruptFlags)
        {}
    USB->USBDevIntClr = aDevInterruptFlags;
}

/*-------------------------------------------------------------------------*
 * Internal Function:  ILPC1756_USBDev_Command
 *---------------------------------------------------------------------------*/
/**
 *  Put a command in the command buffer after clearing the command
 *      related device interrupt flags.
 *
 *  @param [in]    *p 			workspace
 *
 *  @param [in]    aCommand  	Command to send
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void ILPC1756_USBDev_Command(
            T_LPC1756_USBDev_Workspace *p,
            TUInt8 aCommand)
{
if (aCommand == USB_ENDPOINT_CMD_VALIDATE_BUFFER)
    dprintf("[Cmd V]");
else if (aCommand == USB_ENDPOINT_CMD_CLEAR_BUFFER)
    dprintf("[Cmd C]");
else if (aCommand <= 0x3F)
    dprintf("[Cmd S%d]", aCommand&0x3F);
else if ((aCommand & 0xC0)==0x40)
    dprintf("[Cmd SC%d]", aCommand&0x3F);
else if (aCommand == USB_DEVICE_CMD_SET_MODE)
    dprintf("[Cmd SM]");
else if (aCommand == USB_DEVICE_CMD_STATUS)
    ; // do nothing
else
    dprintf("[Cmd ?%02X]", aCommand);
    // Start by clearing the full and empty flags for the
    // command.
    USB->USBDevIntClr = CDFULL | CCEMTY;

    // Write a command in the form of:
    //    0x00CC0500
    //    where CC = 8-bit command
    USB->USBCmdCode = 0x00000500 | (aCommand << 16);

    // Wait for there to be room for another command
    IWaitForDeviceInterrupt(p, CCEMTY);
}

/*-------------------------------------------------------------------------*
 * Internal Function:  ILPC1756_USBDev_CommandAndWrite
 *---------------------------------------------------------------------------*/
/**
 *  Write a command and then write the data after it.
 *
 *  @param [in]    *p 			workspace
 *
 *  @param [in]    aCommand  	Command to send
 *
 *  @param [in]    aData     	Data to go after command
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void ILPC1756_USBDev_CommandAndWrite(
                T_LPC1756_USBDev_Workspace *p,
                TUInt8 aCommand,
                TUInt16 aData)
{
    // First, write the command
    ILPC1756_USBDev_Command(p, aCommand);

    // Then, write the data
    USB->USBCmdCode = 0x00000100 | (aData << 16);

    // Wait for room to be made available for more data
    IWaitForDeviceInterrupt(p, CCEMTY);
}

/*-------------------------------------------------------------------------*
 * Internal Function:  ILPC1756_USBDev_CommandAndRead
 *---------------------------------------------------------------------------*/
/**
 *  Write a command and then read the data after it.
 *
 *  @param [in]    *p			workspace
 *
 *  @param [in]    aCommand    	Command to send
 *
 *  @return        TUInt8     	Data received
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static TUInt8 ILPC1756_USBDev_CommandAndRead(
                T_LPC1756_USBDev_Workspace *p,
                TUInt8 aCommand)
{
    // First, write a command
    ILPC1756_USBDev_Command(p, aCommand);

    // Second, read back a byte
    USB->USBCmdCode = 0x00000200 | (aCommand << 16);

    // Wait until data is ready to be read
    IWaitForDeviceInterrupt(p, CDFULL);

    return (TUInt8)USB->USBCmdData;
}

/*-------------------------------------------------------------------------*
 * Function:  LPC1756_USBDev_InterruptNakEnable
 *---------------------------------------------------------------------------*/
/**
 *  Declares which NAK bits to produce interrupts on.
 *
 *  @param [in]    *aWorkspace 		workspace
 *
 *  @param [in]    aNakBits    		Or'd list of NAK bits to interrupt on
 *       
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void LPC1756_USBDev_InterruptNakEnable(
            void *aWorkspace,
            TUInt8 aNakBits)
{
    T_LPC1756_USBDev_Workspace *p = (T_LPC1756_USBDev_Workspace *)aWorkspace;

    ILPC1756_USBDev_CommandAndWrite(p, USB_DEVICE_CMD_SET_MODE, aNakBits);
}

/*-------------------------------------------------------------------------*
 * Function:  LPC1756_USBDev_EndpointInterruptsEnable
 *---------------------------------------------------------------------------*/
/**
 *  Allow an endpoint to generate interrupts.
 *
 *  @param [in]    *aWorkspace 	workspace
 *
 *  @param [in]    aEndpoint  	Endpoint to trigger interrupts
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void LPC1756_USBDev_EndpointInterruptsEnable(
                void *aWorkspace,
                TUInt8 aEndpoint)
{
    TUInt32 aIndex = IEndpointToIndex(aEndpoint);

    // Enable interrupts for this endpoint
    // using regular "slow" method
    USB->USBEpIntEn |= (1 << aIndex);
    USB->USBDevIntEn |= EP_SLOW;
}

/*-------------------------------------------------------------------------*
 * Function:  LPC1756_USBDev_EndpointInterruptsDisable
 *---------------------------------------------------------------------------*/
/**
 *  Allow an endpoint to generate interrupts.
 *
 *  @param [in]    *aWorkspace 		workspace
 *
 *  @param [in]    aEndpoint    	Endpoint to trigger interrupts
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void LPC1756_USBDev_EndpointInterruptsDisable(
                void *aWorkspace,
                TUInt8 aEndpoint)
{
    TUInt32 aIndex = IEndpointToIndex(aEndpoint);

    // Disable interrupts for this endpoint
    USB->USBEpIntEn &= ~(1 << aIndex);
}


/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_USBDev_InitializeWorkspace
 *---------------------------------------------------------------------------*/
/**
 *  Setup the workspace for this USBDev Controller
 *
 *  @param [in]    *aWorkspace       	Memory allocated for workspace area.
 *
 *  @return        T_uezError       	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError LPC1756_USBDev_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1756_USBDev_Workspace *p = (T_LPC1756_USBDev_Workspace *)aWorkspace;

    // Nothing is defined in this case
    p->iSettings.iUSBDevice = 0;
    p->iSettings.iUSBDescriptorTable = 0;

    // Remember this for the interrupt
    G_LPC1756_USBDev_Workspace = p;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_USBDev_Configure
 *---------------------------------------------------------------------------*/
/**
 *  Configure and setup the USBDev screen with the given settings
 *
 *  @param [in]    *aWorkspace    	USBDev Controller's workspace
 *
 *  @param [in]    *iSettings 		Pointer to settings to use
 *
 *  @return        T_uezError     	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError LPC1756_USBDev_Configure(
        void *aWorkspace,
        T_USBDeviceControllerSettings *iSettings)
{
    T_LPC1756_USBDev_Workspace *p = (T_LPC1756_USBDev_Workspace *)aWorkspace;

    // Copy over the configuration
    p->iSettings = *iSettings;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC1756_USBDev_InterruptProcess
 *---------------------------------------------------------------------------*/
/**
 *  Process USB Device Interrupts
 *
 *  @param [in]    *p  	USBDev Controller's workspace
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void ILPC1756_USBDev_InterruptProcess(T_LPC1756_USBDev_Workspace *p)
{
    TUInt32 status;
    T_USBStatus usbStatus;
    T_USBDevice *p_dev = p->iSettings.iUSBDevice;

    status = USB->USBDevIntSt;

    if (status & DEV_STAT) {
        // Clear the device status change flag
        USB->USBDevIntClr = DEV_STAT;

        if (p_dev) {
            // Read the device status and run the callback (if it exists)
            usbStatus = (T_USBStatus)ILPC1756_USBDev_CommandAndRead(
                            p,
                            USB_DEVICE_CMD_STATUS);
            if (p_dev->iStatusChangeFunc)
                p_dev->iStatusChangeFunc(
                    p_dev->iCallbackWorkspace,
                    usbStatus);
        }
    }

    // Handle Endpoints (SLOW)
    if (status & EP_SLOW) {

#if 0
        USB->USBDevIntClr = EP_SLOW;
        // Handle endpoints with data on them
        for (i = 0; i < 32; i++) {
            if (USB->USBEpIntSt & (1<<i)) {
                // Clear the interrupt flag
                USB->USBEpIntClr = (1<<i);

                // Be sure wait for a command to be available
                IWaitForDeviceInterrupt(p, CDFULL);

                // Get the end point's status
                endpointStatus = USB->USBCmdData;

                // Call the end point handler
                if (i & 1)  {
                    if (p_dev->iEndpointFuncs[i>>1].iIn) {
                        p_dev->iEndpointFuncs[i>>1].iIn(
                            p_dev->iCallbackWorkspace,
                            IIndexToEndpoint(i),
                            endpointStatus);
                    } else {
                        // TBD: Clear the endpoint?
                    }
                } else {
                    if (p_dev->iEndpointFuncs[i>>1].iOut) {
                        p_dev->iEndpointFuncs[i>>1].iOut(
                            p_dev->iCallbackWorkspace,
                            IIndexToEndpoint(i),
                            endpointStatus);
                    } else {
                        // TBD: Clear the endpoint?
                    }
                }
            }
        }
#else
        if (p_dev->iStatusEndpointReadyFunc) {
            InterruptDisable(USB_IRQn);
            p_dev->iStatusEndpointReadyFunc(
                p_dev->iCallbackWorkspace);
        }
#endif
    }

    // USB_DEV_FRAME status?
    if (status & USB_DEV_FRAME) {
        // Clear USB_DEV_FRAME flag
        USB->USBDevIntClr = USB_DEV_FRAME;

        // Do nothing for now
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_USBDev_ProcessEndpoints
 *---------------------------------------------------------------------------*/
/**
 *  Determine if an endpoint is ready and if it is, call the function
 *      associated with that endpoint.  Clear any related interrupts.
 *
 *  @param [in]    *aWorkspace   	USBDev Controller's workspace
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void LPC1756_USBDev_ProcessEndpoints(void *aWorkspace)
{
    T_LPC1756_USBDev_Workspace *p = (T_LPC1756_USBDev_Workspace *)aWorkspace;
    TUInt32 i;
    T_USBEndpointStatus endpointStatus;
    T_USBDevice *p_dev = p->iSettings.iUSBDevice;

    USB->USBDevIntClr = EP_SLOW;

    // Handle endpoints with data on them
    for (i = 0; i < 32; i++) {
        if (USB->USBEpIntSt & (1<<i)) {
            // Clear the interrupt flag
            USB->USBEpIntClr = (1<<i);

            if (p_dev) {
                // Be sure wait for a command to be available
                IWaitForDeviceInterrupt(p, CDFULL);

                // Get the end point's status
                endpointStatus = USB->USBCmdData;

                // Call the end point handler
                if (i & 1)  {
    dprintf("\n[IN%d]", i>>1);
                    if (p_dev->iEndpointFuncs[i>>1].iIn) {
                        p_dev->iEndpointFuncs[i>>1].iIn(
                            p_dev->iCallbackWorkspace,
                            IIndexToEndpoint(i),
                            endpointStatus);
                    } else {
                        // TBD: Clear the endpoint?
                    }
                } else {
    dprintf("\n[OUT%d]", i>>1);
                    if (p_dev->iEndpointFuncs[i>>1].iOut) {
                        p_dev->iEndpointFuncs[i>>1].iOut(
                            p_dev->iCallbackWorkspace,
                            IIndexToEndpoint(i),
                            endpointStatus);
                    } else {
                        // TBD: Clear the endpoint?
                    }
                }
            }
        }
    }

    // Ensure the interrupts are back on
    InterruptEnable(USB_IRQn);
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC1756_USBDev_InterruptVector
 *---------------------------------------------------------------------------*/
/**
 *  Process USB Device Interrupts
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void LPC1756_InterruptProcess_USBDevice(void)
{
    if (G_LPC1756_USBDev_Workspace)
        ILPC1756_USBDev_InterruptProcess(G_LPC1756_USBDev_Workspace);
}

/*-------------------------------------------------------------------------*
 * Function:  LPC1756_USBDev_Connect
 *---------------------------------------------------------------------------*/
/**
 *  Connect to the USB bus.
 *
 *  @param [in]    *aWorkspace   	USBDev Controller's workspace
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void LPC1756_USBDev_Connect(void *aWorkspace)
{
    ILPC1756_USBDev_CommandAndWrite(
        (T_LPC1756_USBDev_Workspace *)aWorkspace,
        USB_DEVICE_CMD_STATUS,
        USB_STATUS_CONNECT);
}

/*-------------------------------------------------------------------------*
 * Function:  LPC1756_USBDev_Disconnect
 *---------------------------------------------------------------------------*/
/**
 *  Disconnect from the USB bus.
 *
 *  @param [in]    *aWorkspace 	USBDev Controller's workspace
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void LPC1756_USBDev_Disconnect(void *aWorkspace)
{
    ILPC1756_USBDev_CommandAndWrite(
        (T_LPC1756_USBDev_Workspace *)aWorkspace,
        USB_DEVICE_CMD_STATUS,
        0x00);
}

/*-------------------------------------------------------------------------*
 * Function:  LPC1756_USBDev_SetAddress
 *---------------------------------------------------------------------------*/
/**
 *  Sets the USB address for this device.
 *
 *  @param [in]    *aWorkspace 	USBDev Controller's workspace
 *
 *  @param [in]    aAddress  	7 bit address of this device.
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void LPC1756_USBDev_SetAddress(void *aWorkspace, TUInt8 aAddress)
{
    ILPC1756_USBDev_CommandAndWrite(
        (T_LPC1756_USBDev_Workspace *)aWorkspace,
        USB_DEVICE_CMD_SET_ADDRESS,
        USB_DEVICE_SET_ADDRESS_ENABLE | aAddress);
    ILPC1756_USBDev_CommandAndWrite(
        (T_LPC1756_USBDev_Workspace *)aWorkspace,
        USB_DEVICE_CMD_SET_ADDRESS,
        USB_DEVICE_SET_ADDRESS_ENABLE | aAddress);
}

/*-------------------------------------------------------------------------*
 * Function:  LPC1756_USBDev_DeviceIsConfigured
 *---------------------------------------------------------------------------*/
/**
 *  Declare the USB Device as configured.
 *
 *  @param [in]    *aWorkspace    	USBDev Controller's workspace
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void LPC1756_USBDev_DeviceIsConfigured(void *aWorkspace)
{
    ILPC1756_USBDev_CommandAndWrite(
            (T_LPC1756_USBDev_Workspace *)aWorkspace,
            USB_DEVICE_CMD_CONFIG,
            USB_DEVICE_CONFIG);
}

/*-------------------------------------------------------------------------*
 * Function:  LPC1756_USBDev_DeviceIsNotConfigured
 *---------------------------------------------------------------------------*/
/**
 *  Declare the USB Device as NOT configured.
 *
 *  @param [in]    *aWorkspace    	USBDev Controller's workspace
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void LPC1756_USBDev_DeviceIsNotConfigured(void *aWorkspace)
{
    ILPC1756_USBDev_CommandAndWrite(
        (T_LPC1756_USBDev_Workspace *)aWorkspace,
        USB_DEVICE_CMD_CONFIG,
        0);
}


/*-------------------------------------------------------------------------*
 * Function:  LPC1756_USBDev_Read
 *---------------------------------------------------------------------------*/
/**
 *  Read data from an endpoint
 *
 *  @param [in]    *aWorkspace     	USBDev Controller's workspace
 *
 *  @param [in]    aEndpoint      	Endpoint with data
 *
 *  @param [in]    *aData          	Pointer to place to put data.
 *
 *  @param [in]    iMaxLen          Maximum number of bytes for data.
 *
 *  @return        TInt16        	Number of bytes received, or -1
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TInt16 LPC1756_USBDev_Read(
            void *aWorkspace,
            TUInt8 aEndpoint,
            TUInt8 *aData,
            TUInt16 iMaxLen)
{
    TUInt32 aIndex = IEndpointToIndex(aEndpoint);
    TUInt32 rxLength;
    TUInt32 rxData;
    T_LPC1756_USBDev_Workspace *p = (T_LPC1756_USBDev_Workspace *)aWorkspace;

dprintf("[R %d %d]", aEndpoint, iMaxLen);
    // Setup the endpoint for reading
    USB->USBCtrl = RD_EN | ((aEndpoint & 0xF) << 2);

    // Wait for the packet to be ready (or an error)
    while (1)  {
        if ((rxLength = USB->USBRxPLen) & PKT_RDY)
            break;
    }

    // Is data valid?
    if (!(rxLength & DV))
        return -1;

    // Data is valid.
    // Clean up and get the length.
    rxLength &= PKT_LNGTH_MASK;

    // Read data while it is available (32 bits at a time).
    while (USB->USBCtrl & RD_EN) {
        // Get the 32 bits (or less)
        rxData = USB->USBRxData;

        // Only store the data if there is a place to put it
        if (aData != 0) {
            // How much room is left?
            if (iMaxLen >= 4)  {
                // Do a group of 4 bytes quickly.
                // NOTE:  Compiler may optimize
                aData[0] = (rxData>>0) & 0xFF;
                aData[1] = (rxData>>8) & 0xFF;
                aData[2] = (rxData>>16) & 0xFF;
                aData[3] = (rxData>>24) & 0xFF;
                iMaxLen -= 4;
                aData += 4;
            } else {
                // Do a group of less than 4 bytes
                while (iMaxLen)  {
                    // One byte at a time
                    *(aData++) = rxData & 0xFF;
                    rxData >>= 8;
                    iMaxLen--;
                }
            }
        }
    }

    // Regardless of what happened above,
    // do a 'selection' and a 'clear' of the buffer
    ILPC1756_USBDev_Command(p, USB_ENDPOINT_CMD_SELECT | aIndex);
    ILPC1756_USBDev_Command(p, USB_ENDPOINT_CMD_CLEAR_BUFFER);

    return rxLength;
}

/*-------------------------------------------------------------------------*
 * Function:  LPC1756_USBDev_Write
 *---------------------------------------------------------------------------*/
/**
 *  Write data to an endpoint
 *
 *  @param [in]    *aWorkspace     	USBDev Controller's workspace
 *
 *  @param [in]    aEndpoint       	Endpoint to receive data
 *
 *  @param [in]    *aData         	Pointer to data to send.
 *
 *  @param [in]    aLength      	Number of bytes to send
 *
 *  @return        TUInt16       	aLength
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TUInt16 LPC1756_USBDev_Write(
                void *aWorkspace,
                TUInt8 aEndpoint,
                TUInt8 *aData,
                TUInt16 aLength)
{
    T_LPC1756_USBDev_Workspace *p = (T_LPC1756_USBDev_Workspace *)aWorkspace;
    TUInt32 aIndex = IEndpointToIndex(aEndpoint);

dprintf("[W %d %d]", aEndpoint, aLength);
    // Enable writing to the end point and the number of bytes to be written
    USB->USBCtrl = WR_EN | ((aEndpoint & 0xF) << 2);
    USB->USBTxPLen = aLength;

    if (aLength == 0)  {
        while (USB->USBCtrl & WR_EN)  {
            // Just force zeroes into without using
            // the pointer.
            USB->USBTxData = 0;
        }
    } else {
        // Write data as long as it lets us
        while (USB->USBCtrl & WR_EN)  {
            // Write in 32 bit blocks
            USB->USBTxData =
                (aData[3] << 24) |
                (aData[2] << 16) |
                (aData[1] << 8) |
                aData[0];
            aData += 4;
        }
    }

    // Do a 'select' and 'validate' on the endpoint
    ILPC1756_USBDev_Command(p, USB_ENDPOINT_CMD_SELECT | aIndex);
    ILPC1756_USBDev_Command(p, USB_ENDPOINT_CMD_VALIDATE_BUFFER);

    return aLength;
}

/*-------------------------------------------------------------------------*
 * Function:  LPC1756_USBDev_SetStallState
 *---------------------------------------------------------------------------*/
/**
 *  Set the stall state of an endpoint
 *
 *  @param [in]    *aWorkspace   USBDev Controller's workspace
 *
 *  @param [in]    aEndpoint     Endpoint to set to stalled or not stalled.
 *
 *  @param [in]    aStallState   ETrue for stall, else EFalse for not stalled.
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void LPC1756_USBDev_SetStallState(
            void *aWorkspace,
            TUInt8 aEndpoint,
            TBool aStallState)
{
dprintf("ST %d %d\n", aEndpoint, aStallState);
    // Write the stall bit of the end point.
    ILPC1756_USBDev_CommandAndWrite(
        (T_LPC1756_USBDev_Workspace *)aWorkspace,
        USB_ENDPOINT_CMD_SET_STATUS | IEndpointToIndex(aEndpoint),
        aStallState ? ENDPOINT_STATUS_STALLED : 0);
}

/*-------------------------------------------------------------------------*
 * Function:  LPC1756_USBDev_IsStalled
 *---------------------------------------------------------------------------*/
/**
 *  Determine if an endpoint is stalled.
 *
 *  @param [in]    *aWorkspace      	USBDev Controller's workspace
 *
 *  @param [in]    aEndpoint        	Endpoint to check if stalled
 *
 *  @return        TBool         		ETrue if stalled, else EFalse
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TBool LPC1756_USBDev_IsStalled(void *aWorkspace, TUInt8 aEndpoint)
{
    // Determine if the endpoint's stalled bit is set.
    return (ILPC1756_USBDev_CommandAndRead(
                (T_LPC1756_USBDev_Workspace *)aWorkspace,
                USB_ENDPOINT_CMD_SELECT |
                    IEndpointToIndex(aEndpoint)) &
                ENDPOINT_STATUS_STALLED);
}

/*-------------------------------------------------------------------------*
 * Function:  LPC1756_USBDev_SetConfiguration
 *---------------------------------------------------------------------------*/
/**
 *  Go through the descriptor table and enable all endpoints for
 *      the given descriptor index and alternative setting.
 *      All corresponding endpoints are configured for matches.
 *
 *  @param [in]    *aWorkspace    		USBDev Controller's workspace
 *
 *  @param [in]    aConfigurationIndex	Descriptor index of config to use.
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void LPC1756_USBDev_SetConfiguration(
                void *aWorkspace,
                TUInt8 aConfigurationIndex)
{
    T_LPC1756_USBDev_Workspace *p_workspace =
            (T_LPC1756_USBDev_Workspace *)aWorkspace;
    const TUInt8 *p = p_workspace->iSettings.iUSBDescriptorTable;
//    TUInt8 configuration = 0x80;

    T_USBDescriptorHeader *p_header = (T_USBDescriptorHeader *)p;

dprintf("\n\n---> [Cf %d]", aConfigurationIndex);

    // Index currently is 0 or non-zero.  If 0, it means
    // we are closing out the configuration.  Is it a zero?
    if (aConfigurationIndex == 0)  {
        // Yes, then kill the configuration
        LPC1756_USBDev_DeviceIsNotConfigured(aWorkspace);
    } else {
        // Otherwise, we are going to use the existing configuration
        while (p_header->iLength)  {
            if (p_header->iDescriptorType == USB_DESCRIPTOR_CONFIGURATION)  {
                // Found a configuration, the following
                // descriptors are now in that configuration
//                configuration =
//                    ((T_USBDescriptorConfiguration *)p)->iConfigurationValue;
            } else if (p_header->iDescriptorType == USB_DESCRIPTOR_ENDPOINT)  {
                // Only configure this endpoint if we have a matching
                // configuration number and alternate setting number.
                #define p_endpoint ((T_USBDescriptorEndpoint *)p)
                LPC1756_USBDev_EndpointConfigure(
                    aWorkspace,
                    p_endpoint->iAddress,
                    p_endpoint->iMaximumPacketSize);
            }
            // Next descriptor
            p += p_header->iLength;
            p_header = (T_USBDescriptorHeader *)p;
        }
        // Got to the end
        // Report that this device is configured.
        LPC1756_USBDev_DeviceIsConfigured((void *)p_workspace);
    }

    // Remember our configuration number
    p_workspace->iConfigurationNumber = aConfigurationIndex;
}

/*-------------------------------------------------------------------------*
 * Function:  LPC1756_USBDev_GetConfiguration
 *---------------------------------------------------------------------------*/
/**
 *  Go through the descriptor table and enable all endpoints for
 *      the given descriptor index and alternative setting.
 *      All corresponding endpoints are configured for matches.
 *
 *  @param [in]    *aWorkspace  USBDev Controller's workspace
 *
 *  @return        TUInt8 		Config Index Descriptor - index of config in use.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static TUInt8 LPC1756_USBDev_GetConfiguration(
                void *aWorkspace)
{
    T_LPC1756_USBDev_Workspace *p_workspace =
            (T_LPC1756_USBDev_Workspace *)aWorkspace;

    // Report our configuration
    return p_workspace->iConfigurationNumber;
}

/*-------------------------------------------------------------------------*
 * Function:  LPC1756_USBDev_ForceAddressAndConfiguration
 *---------------------------------------------------------------------------*/
/**
 *  Force the USB to go to a given address and configuration.
 *
 *  @param [in]    *aWorkspace     	USBDev Controller's workspace
 *
 *  @param [in]    aAddress      	Address of connection
 *
 *  @param [in]    aConfigNum   	Number of configuration to use
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void LPC1756_USBDev_ForceAddressAndConfiguration(
            void *aWorkspace,
            TUInt8 aAddress,
            TUInt8 aConfigNum)
{
    T_LPC1756_USBDev_Workspace *p = (T_LPC1756_USBDev_Workspace *)aWorkspace;

    LPC1756_USBDev_SetAddress(aWorkspace, aAddress);
    LPC1756_USBDev_SetConfiguration(aWorkspace, aConfigNum);
    p->iConfigurationNumber = aConfigNum;
}

/*---------------------------------------------------------------------------*
 * Internal Function:  IEndpointEnable
 *---------------------------------------------------------------------------*/
/**
 *  Enable endpoint.
 *
 *  @param [in]    *p 			workspace
 *
 *  @param [in]    aIndex    	Index of endpoint
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void LPC1756_USBDev_EndpointEnable(
                T_LPC1756_USBDev_Workspace *p,
                int aIndex)
{
    ILPC1756_USBDev_CommandAndWrite(
        p,
        USB_ENDPOINT_CMD_SET_STATUS | aIndex,
        0);
}

/*-------------------------------------------------------------------------*
 * Internal Function:  LPC1756_USBDev_EndpointDisable
 *---------------------------------------------------------------------------*/
/**
 *  Disable endpoint.
 *
 *  @param [in]    *p 			workspace
 *
 *  @param [in]    aIndex    	Index of endpoint
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
#if 0
static void LPC1756_USBDev_EndpointDisable(
                T_LPC1756_USBDev_Workspace *p,
                int aIndex)
{
    ILPC1756_USBDev_CommandAndWrite(
            p,
            USB_ENDPOINT_CMD_SET_STATUS | aIndex,
            ENDPOINT_STATUS_DISABLED);
}
#endif

/*-------------------------------------------------------------------------*
 * Internal Function:  LPC1756_USBDev_EndpointRealize
 *---------------------------------------------------------------------------*/
/**
 *  Realize an endpoint by telling the hardware to reserve space and
 *      to set the maximum packet size.
 *
 *  @param [in]    *p 				workspace
 *
 *  @param [in]    aIndex         	Index of endpoint
 *
 *  @param [in]    aMaxPacketSize	Maximum size of packet (in bytes)
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void LPC1756_USBDev_EndpointRealize(
                T_LPC1756_USBDev_Workspace *p,
                TUInt32 aIndex,
                TUInt16 aMaxPacketSize)
{
    // Declare which endpoint is going to be realized
    // using a bit position.
    USB->USBReEp |= (1 << aIndex);
    USB->USBEpInd = aIndex;

    // Declare size of maximum packets
    USB->USBMaxPSize = aMaxPacketSize;

    // Wait for endpoint to be fully realized
    IWaitForDeviceInterrupt(p, EP_RLZED);
}

/*-------------------------------------------------------------------------*
 * Function:  LPC1756_USBDev_EndpointConfigure
 *---------------------------------------------------------------------------*/
/**
 *  Realize and enable an endpoint.
 *
 *  @param [in]    *aWorkspace      USBDev Controller's workspace
 *
 *  @param [in]    aEndpoint      	Endpoint identifier
 *
 *  @param [in]    aMaxPacketSize  	Maximum size of packets
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void LPC1756_USBDev_EndpointConfigure(
            void *aWorkspace,
            TUInt8 aEndpoint,
            TUInt16 aMaxPacketSize)
{
    TUInt32 aIndex = IEndpointToIndex(aEndpoint);
    T_LPC1756_USBDev_Workspace *p = (T_LPC1756_USBDev_Workspace *)aWorkspace;

    // realise EP
    LPC1756_USBDev_EndpointRealize(p, aIndex, aMaxPacketSize);

    // enable EP
    LPC1756_USBDev_EndpointEnable(
            p,
            aIndex);
}

/*-------------------------------------------------------------------------*
 * Function:  USBInitialize
 *---------------------------------------------------------------------------*/
/**
 *  Initialize the hardware
 *
 *  @param [in]    *aWorkspace    	USBDev Controller's workspace
 *
 *  @return        
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void LPC1756_USBDev_Initialize(void *aWorkspace)
{
    extern void USBSharedInterruptSetup(void);
    TUInt8 i;
    T_LPC1756_USBDev_Workspace *p = (T_LPC1756_USBDev_Workspace *)aWorkspace;

    // Ensure power is on
    SC->PCONP |= 0x80000000;

    USB->u1.OTGClkCtrl = 0x0000001B;
    while ((USB->u2.OTGClkSt & 0x0000001B) != 0x1B) {
        ;
    }
    USB->OTGStCtrl = 0;

    // Setup standard control endpoints
    LPC1756_USBDev_EndpointConfigure(
        aWorkspace,
        0x00,
        USB_ENDPOINT0_PACKET_SIZE);
    LPC1756_USBDev_EndpointConfigure(
        aWorkspace,
        0x80,
        USB_ENDPOINT0_PACKET_SIZE);

    // Only ACKs generate interrupts
//    LPC1756_USBDev_InterruptNakEnable(aWorkspace, 0);
    LPC1756_USBDev_InterruptNakEnable(aWorkspace, USB_DEVICE_SET_MODE_INAK_BI);

    // Setup interrupts for endpoints
    for (i=0; i<MAX_NUM_ENDPOINTS; i++)  {
        if (p->iSettings.iUSBDevice->iEndpointFuncs[i].iIn)
            LPC1756_USBDev_EndpointInterruptsEnable(aWorkspace,
                USB_ENDPOINT_ADDRESS(i, USB_ENDPOINT_ADDRESS_DIR_IN));
        if (p->iSettings.iUSBDevice->iEndpointFuncs[i].iOut)
            LPC1756_USBDev_EndpointInterruptsEnable(aWorkspace,
                USB_ENDPOINT_ADDRESS(i, USB_ENDPOINT_ADDRESS_DIR_OUT));
    }

    USBSharedInterruptSetup();
}

/*-------------------------------------------------------------------------*
 * Function:  LPC1756_USBDev_GetDescriptor
 *---------------------------------------------------------------------------*/
/**
 *  Search for a descriptor in the descriptor table based on the
 *      type and the index (of that type).  Return a pointer to the data
 *      and fill in the length of the data.
 *
 *  @param [in]    *aWorkspace  USBDev Controller's workspace
 *
 *  @param [in]    aType     	Type of descriptor to find
 *
 *  @param [in]    aIndex     	Index to descriptor (zero based)
 *
 *  @param [in]    *aLength   	Pointer to place to store length
 *
 *  @return        *TUInt8     	Pointer to descriptor, or 0 for none.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
const TUInt8 *LPC1756_USBDev_GetDescriptor(
            void *aWorkspace,
            TUInt8 aType,
            TUInt16 aIndex,
            TUInt16 *aLength)
{
    const TUInt8 *p = ((T_LPC1756_USBDev_Workspace *)aWorkspace)->
                    iSettings.iUSBDescriptorTable;
    T_USBDescriptorHeader *p_header = (T_USBDescriptorHeader *)p;

dprintf("[GetDesc %d %d]", aType, aIndex);
    while (p_header->iLength)  {
dprintf("<%p %d %d>", p_header, p_header->iDescriptorType, p_header->iLength);
        if (p_header->iDescriptorType == aType)  {
            if (aIndex==0)  {
                // Found the nth descriptor of the given type
                // Are we a configuration type?
                if (aType == USB_DESCRIPTOR_CONFIGURATION)  {
                    *aLength = ((T_USBDescriptorConfiguration *)p)->iTotalLength;
                } else {
                    *aLength = p_header->iLength;
                }
dprintf("[Found %p]\n", p);
                return p;
            } else {
                // Found the right type, but wrong index.
                // Keep looking.
                aIndex--;
            }
        }
        // Next descriptor
        p += p_header->iLength;
        p_header = (T_USBDescriptorHeader *)p;
    }
dprintf("[Not Found]\n");
    // If we got here, we never found it.
    return 0;
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_USBDeviceController LPC1756_USBDev_Interface = {
	{
	    "NXP:LPC1756:USB Device Controller",
	    0x0100,
	    LPC1756_USBDev_InitializeWorkspace,
	    sizeof(T_LPC1756_USBDev_Workspace),
	},
	
    LPC1756_USBDev_Configure,
    LPC1756_USBDev_Initialize,
    LPC1756_USBDev_Connect,
    LPC1756_USBDev_Disconnect,
    LPC1756_USBDev_SetAddress,
    LPC1756_USBDev_SetConfiguration,
    LPC1756_USBDev_GetConfiguration,
    LPC1756_USBDev_EndpointConfigure,
    LPC1756_USBDev_Read,
    LPC1756_USBDev_Write,
    LPC1756_USBDev_SetStallState,
    LPC1756_USBDev_IsStalled,
    LPC1756_USBDev_EndpointInterruptsEnable,
    LPC1756_USBDev_EndpointInterruptsDisable,
    LPC1756_USBDev_ForceAddressAndConfiguration,
    LPC1756_USBDev_ProcessEndpoints,
    LPC1756_USBDev_GetDescriptor,
    LPC1756_USBDev_InterruptNakEnable,
};

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC1756_USBDevice_Require(
        const T_LPC1756_USBDevice_Settings *aSettings)
{
    static const T_LPC1756_IOCON_ConfigList pinsVBUS[] = {
            {GPIO_P1_30, IOCON_A_DEFAULT(2)}, // USB_VBUS
    };
    static const T_LPC1756_IOCON_ConfigList pinsDP[] = {
            {GPIO_P0_29, IOCON_U_DEFAULT(1)}, // USB_D+
    };
    static const T_LPC1756_IOCON_ConfigList pinsDM[] = {
            {GPIO_P0_30, IOCON_U_DEFAULT(1)}, // USB_D-
    };
    static const T_LPC1756_IOCON_ConfigList pinsCONNECT[] = {
            {GPIO_P2_9,  IOCON_D_DEFAULT(1)}, // USB_CONNECT
    };
    static const T_LPC1756_IOCON_ConfigList pinsUP_LED[] = {
            {GPIO_P1_18, IOCON_D_DEFAULT(1)}, // USB_UP_LED
    };

    HAL_DEVICE_REQUIRE_ONCE();
    // Register USB Device
    HALInterfaceRegister("USBDeviceController",
            (T_halInterface *)&LPC1756_USBDev_Interface, 0, 0);

    LPC1756_IOCON_ConfigPinOrNone(aSettings->iVBUS, pinsVBUS, ARRAY_COUNT(pinsVBUS));
    LPC1756_IOCON_ConfigPin(aSettings->iDP, pinsDP, ARRAY_COUNT(pinsDP));
    LPC1756_IOCON_ConfigPin(aSettings->iDM, pinsDM, ARRAY_COUNT(pinsDM));
    LPC1756_IOCON_ConfigPinOrNone(aSettings->iCONNECT, pinsCONNECT,
            ARRAY_COUNT(pinsCONNECT));
    LPC1756_IOCON_ConfigPinOrNone(aSettings->iUP_LED, pinsUP_LED,
            ARRAY_COUNT(pinsUP_LED));
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC1756_USBDeviceController.c
 *-------------------------------------------------------------------------*/

