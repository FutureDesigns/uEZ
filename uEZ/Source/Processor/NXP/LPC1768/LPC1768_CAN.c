/*-------------------------------------------------------------------------*
 * File:  LPC1768_CAN.c
 *-------------------------------------------------------------------------*
 * Description:
 *     TODO change for LPC1768
 *-------------------------------------------------------------------------*/

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
#include <string.h>
#include <uEZ.h>
#include <uEZProcessor.h>
#include <uEZPlatform.h>
#include <uEZGPIO.h>
#include <HAL/Interrupt.h>
#include <Types/CAN.h>
#include "LPC1768_CAN.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    TVUInt32 iMOD;  // 0x00 - Mode Register
    TVUInt32 iCMR;  // 0x04 - Command Register
    TVUInt32 iGSR;  // 0x08 - Global Status Register
    TVUInt32 iICR;  // 0x0C - Interrupt and Capture Register
    TVUInt32 iIER;  // 0x10 - Interrupt Enable Register
    TVUInt32 iBTR;  // 0x14 - Bus Timing Register
    TVUInt32 iEWL;  // 0x18 - Error Warning Limit Register
    TVUInt32 iSR;   // 0x1C - Status Register
    TVUInt32 iRFS;  // 0x20 - Receive Frame Status Register
    TVUInt32 iRID;  // 0x24 - Receive Identifier Register
    TVUInt32 iRDA;  // 0x28 - Receive Data Register A
    TVUInt32 iRDB;  // 0x2C - Receive Data Register B
    TVUInt32 iTFI1; // 0x30 - Transmit Frame Info Register 1
    TVUInt32 iTID1; // 0x34 - Transmit Identifier Register 1
    TVUInt32 iTDA1; // 0x38 - Transmit Data Register A 1
    TVUInt32 iTDB1; // 0x3C - Transmit Data Register B 1
    TVUInt32 iTFI2; // 0x40 - Transmit Frame Info Register 2
    TVUInt32 iTID2; // 0x44 - Transmit Identifier Register 2
    TVUInt32 iTDA2; // 0x48 - Transmit Data Register A 2
    TVUInt32 iTDB2; // 0x4C - Transmit Data Register B 2
    TVUInt32 iTFI3; // 0x50 - Transmit Frame Info Register 3
    TVUInt32 iTID3; // 0x54 - Transmit Identifier Register 3
    TVUInt32 iTDA3; // 0x58 - Transmit Data Register A 3
    TVUInt32 iTDB3; // 0x5C - Transmit Data Register B 3
} T_LPC1768_CAN_Registers;

typedef struct {
        const HAL_CAN *iHAL;
        T_LPC1768_CAN_Registers *iReg;
        T_CAN_Settings iSettings;
        TUInt8 iCanNum;
}T_LPC1768_CAN_Workspace;
/*---------------------------------------------------------------------------*
 * Function Prototypes:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static T_LPC1768_CAN_Workspace *G_CAN1Workspace;
static T_LPC1768_CAN_Workspace *G_CAN2Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  ILPC1768_CAN_ISR
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1788 CAN workspace.
 * Inputs:
 *      T_LPC1768_ADC_Workspace *aW -- CAN Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static void ILPC1768_CAN_ISR(T_LPC1768_CAN_Workspace *aW)
{
    T_LPC1768_CAN_Registers *p = aW->iReg;
    TUInt32 wordA, wordB;
    T_CAN_Message canMessage;
    
    canMessage.iID = p->iRID;
    canMessage.iDataLen = (p->iRFS >> 16); // DLC
    
    wordA = p->iRDA;
    wordB = p->iRDB;
    
    canMessage.iData[0] = wordA >> 0;
    canMessage.iData[1] = wordA >> 8;
    canMessage.iData[2] = wordA >> 16;
    canMessage.iData[3] = wordA >> 24;
    canMessage.iData[4] = wordB >> 0;
    canMessage.iData[5] = wordB >> 8;
    canMessage.iData[6] = wordB >> 16;
    canMessage.iData[7] = wordB >> 24;

    if(aW->iSettings.iReceiveCallback)
        aW->iSettings.iReceiveCallback(canMessage);
        
    p->iCMR = 0x00000004; /* Release the receive buffer */
}

/*---------------------------------------------------------------------------*
 * Interrupt Routine:  ILPC1768_CAN_Interrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Catch CAN interrupts and use the ISR routine for it
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(ILPC1768_CAN_Interrupt)
{
    IRQ_START();
    
    if((G_CAN1Workspace) && (G_CAN1Workspace->iReg->iGSR & 0x00000001))
        ILPC1768_CAN_ISR(G_CAN1Workspace);
        
    if((G_CAN2Workspace) && (G_CAN2Workspace->iReg->iGSR & 0x00000001))
        ILPC1768_CAN_ISR(G_CAN2Workspace);
    
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Interrupt Routine:  LPC1768_CAN_Send_CAN
 *---------------------------------------------------------------------------*
 * Description:
 *      
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_CAN_Send(
    void *aWorkspace,
    TUInt16 aID,
    TUInt8 *aData,
    TUInt8 aDataLen)
{
    T_LPC1768_CAN_Workspace *aW = (T_LPC1768_CAN_Workspace *)aWorkspace;
    T_LPC1768_CAN_Registers *p = aW->iReg;
    
    if(aDataLen > 8)
        return UEZ_ERROR_INVALID_PARAMETER;
    
    p->iTID1 = aID;
    p->iTFI1 = aDataLen<<16;
    p->iTDA1 = aData[0] + (aData[1]<<8) + (aData[2]<<16) + (aData[3]<<24);
    p->iTDB1 = aData[4] + (aData[5]<<8) + (aData[6]<<16) + (aData[7]<<24);
    
    p->iCMR  = 0x00000021;  // Transmit the message
    
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Interrupt Routine:  LPC1768_CAN_Enable_CAN
 *---------------------------------------------------------------------------*
 * Description:
 *      
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_CAN_Activate(void *aWorkspace)
{
    T_LPC1768_CAN_Workspace *aW = (T_LPC1768_CAN_Workspace *)aWorkspace;
    T_LPC1768_CAN_Registers *p = aW->iReg;
    
    p->iIER   |= (1<<0); // Enable the Receive interrupt
    
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Interrupt Routine:  LPC1768_CAN_Disable_CAN
 *---------------------------------------------------------------------------*
 * Description:
 *      
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_CAN_Deactivate(void *aWorkspace)
{
    T_LPC1768_CAN_Workspace *aW = (T_LPC1768_CAN_Workspace *)aWorkspace;
    T_LPC1768_CAN_Registers *p = aW->iReg;
    
    p->iIER = 0x00000000; // Disable the Receive interrupt
    
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Interrupt Routine:  ILPC1768_CAN_Interrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Catch CAN interrupts and use the ISR routine for it
 *---------------------------------------------------------------------------*/
static void ICANSetBaudRate(
    T_LPC1768_CAN_Workspace *aW,
    TUInt32 aBaudRate)
{
    T_LPC1768_CAN_Registers *p = aW->iReg;
    
    TUInt32 result, BRP=0;
    TUInt8 TSEG1=0, TSEG2=0, NT;

    p->iMOD = 0x00000001; // Set CAN controller into reset
    
    result = PCLK_FREQUENCY / aBaudRate; // Hz

	// Calculate suitable nominal time value
	// NT (nominal time) = (TSEG1 + TSEG2 + 3)
	// NT <= 24
	// TSEG1 >= 2*TSEG2
	for(NT = 24; NT > 0; NT = NT-2) {
		if ((result%NT) == 0) {
			BRP = result / NT - 1;
			NT--;
			TSEG2 = (NT/3) - 1;
			TSEG1 = NT -(NT/3) - 1;
			break;
		}
	}
    
    // set the Bus Timing Register
    p->iBTR = (TSEG2 << 20) | (TSEG1 << 16) | (3 << 14) | BRP;
    
    p->iMOD   = 0x00000000; // Release CAN controller
}

/*---------------------------------------------------------------------------*
 * Interrupt Routine:  ICANSetupExplicitStandardAF
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup explicit standard address filter
 *---------------------------------------------------------------------------*/
static T_uezError ICANSetupExplicitStandardAF(
    T_LPC1768_CAN_Workspace *aW,
    T_CAN_SFF_Entry *aSFFEntries,
    TUInt32 aNumEntries)
{  
    // This routine was ported from the NXP lpc177x_8x_can.c driver
    // Please refer to it if we need to add other filter types
    // For now, we're only doing explicit standard AF for simplicity
    TUInt16 CANAF_std_cnt=0;
    TUInt8 ctrl1, dis1;
    TUInt16 SID, ID_temp,i, count = 0;
    TUInt32 entry, buf;
    
    LPC_CANAF->AFMR = 0x01;
    
    for (i=0;i<aNumEntries;i++) {
    
        if (count + 1 > 512) {
            return UEZ_ERROR_OUT_OF_MEMORY;
        }

        if (aW->iCanNum == 1)
            ctrl1 = 0;
        else
            ctrl1 = 1;

        SID = aSFFEntries[i].iID;

        if(aSFFEntries[i].iEnable)
            dis1 = 0;
        else
            dis1 = 1;

        entry = 0x00; //reset entry value

        if ((CANAF_std_cnt & 0x00000001)==0) {
            if (CANAF_std_cnt !=0 ) {
                buf = LPC_CANAF_RAM->mask[count-1];
                ID_temp = (buf & 0xE7FF); //mask controller & identifier bits
                
                if (ID_temp > ((ctrl1<<13)|SID)) {
                    return UEZ_ERROR_OUT_OF_RANGE;
                }
            }

            entry = (ctrl1<<29)|(dis1<<28)|(SID<<16);

            LPC_CANAF_RAM->mask[count] &= 0x0000FFFF;

            LPC_CANAF_RAM->mask[count] |= entry;

            CANAF_std_cnt++;
            if (CANAF_std_cnt == aNumEntries)//if this is the last SFF entry
                count++;
            
        } else {
            buf = LPC_CANAF_RAM->mask[count];
            ID_temp = (buf >>16) & 0xE7FF;
            
            if (ID_temp > ((ctrl1<<13)|SID)) {
                return UEZ_ERROR_OUT_OF_RANGE;
            }

            entry = (ctrl1 << 13) | (dis1 << 12) | (SID << 0);

            LPC_CANAF_RAM->mask[count] &= 0xFFFF0000;

            LPC_CANAF_RAM->mask[count] |= entry;

            count++;

            CANAF_std_cnt++;
        }
    }
    
    //update address values
    LPC_CANAF->SFF_sa = 0;
	LPC_CANAF->SFF_GRP_sa = LPC_CANAF->SFF_sa + (((CANAF_std_cnt+1)>>1)<< 2);
	LPC_CANAF->EFF_sa = LPC_CANAF->SFF_GRP_sa;
	LPC_CANAF->EFF_GRP_sa = LPC_CANAF->EFF_sa;
	LPC_CANAF->ENDofTable = LPC_CANAF->EFF_GRP_sa;
    
    LPC_CANAF->AFMR = 0x00; // Normal mode
    
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC1768_CAN_SetCANSettings
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the CAN port settings
 * Inputs:
 *      void *aWorkspace          -- CAN port workspace
 *      T_CAN_Settings *aSettings -- Settings to use for CAN port
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_CAN_SetSettings(
        void *aWorkspace,
        T_CAN_Settings *aSettings)
{
    T_LPC1768_CAN_Workspace *aW = (T_LPC1768_CAN_Workspace *)aWorkspace;
    
    memcpy(&aW->iSettings, aSettings, sizeof(T_CAN_Settings));
    
    ICANSetBaudRate(aW, aSettings->iBaud);
    
    // need to setup acceptance filter here
    if(aW->iSettings.iNumSFFEntries > 0)
        ICANSetupExplicitStandardAF(aW, aW->iSettings.iSFFEntry, aW->iSettings.iNumSFFEntries);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC1768_CAN_GetCANSettings
 *---------------------------------------------------------------------------*
 * Description:
 *      Return the previously configured CAN port
 * Inputs:
 *      void *aWorkspace          -- CAN port workspace
 *      T_CAN_Settings *aSettings -- Settings used for CAN port
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_CAN_GetSettings(
        void *aWorkspace,
        T_CAN_Settings *aSettings)
{
    T_LPC1768_CAN_Workspace *aW = (T_LPC1768_CAN_Workspace *)aWorkspace;

    // Record these settings
    *aSettings = aW->iSettings;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Interrupt Routine:  LPC1768_CAN_Send_CAN
 *---------------------------------------------------------------------------*
 * Description:
 *      
 *---------------------------------------------------------------------------*/
 T_uezError LPC1768_CAN_Configure(
        void *aWorkspace,
        void *aCallbackWorkspace)
{
    T_LPC1768_CAN_Workspace *aW = (T_LPC1768_CAN_Workspace *)aWorkspace;
    T_LPC1768_CAN_Registers *p = aW->iReg;

    p->iMOD = 0x00000001;           // Set CAN controller into reset
    p->iIER = 0x00000000;           // Disable the Receive interrupt
	ICANSetBaudRate(aW, 1000000);   // Set baud rate to 1MHz - default
    LPC_CANAF->AFMR = 0x00000002;   // Turn OFF acceptance filters to receive CAN traffic
    p->iMOD   = 0x00000000;         // Release CAN controller

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_CAN_CAN1_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1788 CAN workspace.
 * Inputs:
 *      void *aW                    -- Particular CAN workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_CAN_CAN1_InitializeWorkspace(void * aWorkspace)
{
    T_LPC1768_CAN_Workspace *aW = (T_LPC1768_CAN_Workspace *)aWorkspace;
    aW->iReg = (T_LPC1768_CAN_Registers *)LPC_CAN1_BASE;
    G_CAN2Workspace = aW;
    
    aW->iCanNum = 1;

    //Register the interrupt for this CAN but don't start it
    InterruptRegister(CAN_IRQn, ILPC1768_CAN_Interrupt,
            INTERRUPT_PRIORITY_NORMAL, "CAN");
    InterruptEnable(CAN_IRQn);
    
    aW->iSettings.iReceiveCallback = 0;
    
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_CAN_CAN2_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1788 CAN workspace.
 * Inputs:
 *      void *aW                    -- Particular CAN workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_CAN_CAN2_InitializeWorkspace(void * aWorkspace)
{
    T_LPC1768_CAN_Workspace *aW = (T_LPC1768_CAN_Workspace *)aWorkspace;
    aW->iReg = (T_LPC1768_CAN_Registers *)LPC_CAN2_BASE;
    G_CAN2Workspace = aW;

    aW->iCanNum = 2;
    
    //Register the interrupt for this CAN but don't start it
    InterruptRegister(CAN_IRQn, ILPC1768_CAN_Interrupt,
            INTERRUPT_PRIORITY_NORMAL, "CAN");
    InterruptEnable(CAN_IRQn);
    
    aW->iSettings.iReceiveCallback = 0;
    
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
 const HAL_CAN LPC1768_CAN_CAN1_Interface = {
        {
                "CAN:LPC1788:CAN1",
                0x0100,
                LPC1768_CAN_CAN1_InitializeWorkspace,
                sizeof(T_LPC1768_CAN_Workspace),
        },

        LPC1768_CAN_Configure,
        LPC1768_CAN_SetSettings,
        LPC1768_CAN_GetSettings,
        LPC1768_CAN_Activate,
        LPC1768_CAN_Deactivate,
        LPC1768_CAN_Send,
};

const HAL_CAN LPC1768_CAN_CAN2_Interface = {
        {
                "CAN:LPC1788:CAN2",
                0x0100,
                LPC1768_CAN_CAN2_InitializeWorkspace,
                sizeof(T_LPC1768_CAN_Workspace),
        },

        LPC1768_CAN_Configure,
        LPC1768_CAN_SetSettings,
        LPC1768_CAN_GetSettings,
        LPC1768_CAN_Activate,
        LPC1768_CAN_Deactivate,
        LPC1768_CAN_Send,
};

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC1768_CAN1_Require(
    T_uezGPIOPortPin aPinRD,
    T_uezGPIOPortPin aPinTD)
{
    static const T_LPC1768_IOCON_ConfigList rd1[] = {
            {GPIO_P0_0,  IOCON_D_DEFAULT(1)},
            {GPIO_P0_21, IOCON_D_DEFAULT(4)},
    };
    static const T_LPC1768_IOCON_ConfigList td1[] = {
            {GPIO_P0_1,  IOCON_D_DEFAULT(1)},
            {GPIO_P0_22, IOCON_D_DEFAULT(4)},
    };
    
    HAL_DEVICE_REQUIRE_ONCE();
   
    HALInterfaceRegister("CAN1", (T_halInterface *)&LPC1768_CAN_CAN1_Interface, 0, 0);

    LPC1768_IOCON_ConfigPinOrNone(aPinRD, rd1, ARRAY_COUNT(rd1));
    LPC1768_IOCON_ConfigPinOrNone(aPinTD, td1, ARRAY_COUNT(td1));
    LPC1768PowerOn(1<<13);
}

void LPC1768_CAN2_Require(
    T_uezGPIOPortPin aPinRD,
    T_uezGPIOPortPin aPinTD)
{
    static const T_LPC1768_IOCON_ConfigList rd2[] = {
            {GPIO_P0_4,  IOCON_D_DEFAULT(2)},
            {GPIO_P2_7,  IOCON_D_DEFAULT(1)},
    };
    static const T_LPC1768_IOCON_ConfigList td2[] = {
            {GPIO_P0_5,  IOCON_D_DEFAULT(2)},
            {GPIO_P2_8,  IOCON_D_DEFAULT(1)},
    };
    
    HAL_DEVICE_REQUIRE_ONCE();

    HALInterfaceRegister("CAN2", (T_halInterface *)&LPC1768_CAN_CAN2_Interface, 0, 0);

    LPC1768_IOCON_ConfigPinOrNone(aPinRD, rd2, ARRAY_COUNT(rd2));
    LPC1768_IOCON_ConfigPinOrNone(aPinTD, td2, ARRAY_COUNT(td2));
    LPC1768PowerOn(1<<14);
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC1768_CAN.c
 *-------------------------------------------------------------------------*/
