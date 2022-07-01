/*-------------------------------------------------------------------------*
 * File:  LPC546xx_CAN.c
 *-------------------------------------------------------------------------*
 * Description:
 *
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!  |
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
#include "LPC546xx_CAN.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
        TVInt32 iCNTL;              /*!< (@ 0x400A4000) CAN control                                     */
        TVInt32 iSTAT;              /*!< (@ 0x400A4004) Status register                                 */
        TVInt32 iEC;                /*!< (@ 0x400A4008) Error counter                                   */
        TVInt32 iBT;                /*!< (@ 0x400A400C) Bit timing register                             */
        TVInt32 iINT;               /*!< (@ 0x400A4010) Interrupt register                              */
        TVInt32 iTEST;              /*!< (@ 0x400A4014) Test register                                   */
        TVInt32 iBRPE;              /*!< (@ 0x400A4018) Baud rate prescaler extension register          */
        TVInt32 iRESERVED0;
        TVInt32 iIF1_CMDREQ;        /*!< (@ 0x400A4020) Message interface command request               */

        union {
          TVInt32 iIF1_CMDMSK_R;    /*!< (@ 0x400A4024) Message interface command mask (read direction) */
          TVInt32 iIF1_CMDMSK_W;    /*!< (@ 0x400A4024) Message interface command mask (write direction)*/
        };
        TVInt32 iIF1_MSK1;          /*!< (@ 0x400A4028) Message interface mask 1                        */
        TVInt32 iIF1_MSK2;          /*!< (@ 0x400A402C) Message interface 1 mask 2                      */
        TVInt32 iIF1_ARB1;          /*!< (@ 0x400A4030) Message interface 1 arbitration 1               */
        TVInt32 iIF1_ARB2;          /*!< (@ 0x400A4034) Message interface 1 arbitration 2               */
        TVInt32 iIF1_MCTRL;         /*!< (@ 0x400A4038) Message interface 1 message control             */
        TVInt32 iIF1_DA1;           /*!< (@ 0x400A403C) Message interface data A1                       */
        TVInt32 iIF1_DA2;           /*!< (@ 0x400A4040) Message interface 1 data A2                     */
        TVInt32 iIF1_DB1;           /*!< (@ 0x400A4044) Message interface 1 data B1                     */
        TVInt32 iIF1_DB2;           /*!< (@ 0x400A4048) Message interface 1 data B2                     */
        TVInt32 iRESERVED1[13];
        TVInt32 iIF2_CMDREQ;        /*!< (@ 0x400A4080) Message interface command request               */

        union {
          TVInt32 iIF2_CMDMSK_R;    /*!< (@ 0x400A4084) Message interface command mask (read direction) */
          TVInt32 iIF2_CMDMSK_W;    /*!< (@ 0x400A4084) Message interface command mask (write direction)*/
        };
        TVInt32 iIF2_MSK1;          /*!< (@ 0x400A4088) Message interface mask 1                        */
        TVInt32 iIF2_MSK2;          /*!< (@ 0x400A408C) Message interface 1 mask 2                      */
        TVInt32 iIF2_ARB1;          /*!< (@ 0x400A4090) Message interface 1 arbitration 1               */
        TVInt32 iIF2_ARB2;          /*!< (@ 0x400A4094) Message interface 1 arbitration 2               */
        TVInt32 iIF2_MCTRL;         /*!< (@ 0x400A4098) Message interface 1 message control             */
        TVInt32 iIF2_DA1;           /*!< (@ 0x400A409C) Message interface data A1                       */
        TVInt32 iIF2_DA2;           /*!< (@ 0x400A40A0) Message interface 1 data A2                     */
        TVInt32 iIF2_DB1;           /*!< (@ 0x400A40A4) Message interface 1 data B1                     */
        TVInt32 iIF2_DB2;           /*!< (@ 0x400A40A8) Message interface 1 data B2                     */
        TVInt32 iRESERVED2[21];
        TVInt32 iTXREQ1;            /*!< (@ 0x400A4100) Transmission request 1                          */
        TVInt32 iTXREQ2;            /*!< (@ 0x400A4104) Transmission request 2                          */
        TVInt32 iRESERVED3[6];
        TVInt32 iND1;               /*!< (@ 0x400A4120) New data 1                                      */
        TVInt32 iND2;               /*!< (@ 0x400A4124) New data 2                                      */
        TVInt32 iRESERVED4[6];
        TVInt32 iIR1;               /*!< (@ 0x400A4140) Interrupt pending 1                             */
        TVInt32 iIR2;               /*!< (@ 0x400A4144) Interrupt pending 2                             */
        TVInt32 iRESERVED5[6];
        TVInt32 iMSGV1;             /*!< (@ 0x400A4160) Message valid 1                                 */
        TVInt32 iMSGV2;             /*!< (@ 0x400A4164) Message valid 2                                 */
        TVInt32 iRESERVED6[6];
        TVInt32 iCLKDIV;            /*!< (@ 0x400A4180) CAN clock divider register                      */
} T_LPC546xx_CAN_Registers;

typedef struct {
        const HAL_CAN *iHAL;
        T_LPC546xx_CAN_Registers *iReg;
        T_CAN_Settings iSettings;
        TUInt8 iCanNum;
}T_LPC546xx_CAN_Workspace;
/*---------------------------------------------------------------------------*
 * Function Prototypes:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static T_LPC546xx_CAN_Workspace *G_CAN0Workspace;
static T_LPC546xx_CAN_Workspace *G_CAN1Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  ILPC546xx_CAN_ISR
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1788 CAN workspace.
 * Inputs:
 *      T_LPC546xx_ADC_Workspace *aW -- CAN Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static void ILPC546xx_CAN_ISR(T_LPC546xx_CAN_Workspace *aW)
{
    T_LPC546xx_CAN_Registers *p = aW->iReg;
    T_CAN_Message canMessage;

    canMessage.iID = p->iIF1_ARB1;
    canMessage.iDataLen = (p->iIF1_MCTRL & 0xF); // DLC

    canMessage.iData[0] = p->iIF2_DA1 >> 0;
    canMessage.iData[1] = p->iIF2_DA1 >> 8;
    canMessage.iData[2] = p->iIF2_DA2 >> 0;
    canMessage.iData[3] = p->iIF2_DA2 >> 8;
    canMessage.iData[4] = p->iIF2_DB1 >> 0;
    canMessage.iData[5] = p->iIF2_DB1 >> 8;
    canMessage.iData[6] = p->iIF2_DB2 >> 0;
    canMessage.iData[7] = p->iIF2_DB2 >> 8;

    if(aW->iSettings.iReceiveCallback)
        aW->iSettings.iReceiveCallback(canMessage);

    p->iIF1_CMDMSK_R |= (1<<2);
}

/*---------------------------------------------------------------------------*
 * Interrupt Routine:  ILPC546xx_CAN_Interrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Catch CAN interrupts and use the ISR routine for it
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(ILPC546xx_CAN_Interrupt)
{
    IRQ_START();

    if((G_CAN0Workspace) && (G_CAN0Workspace->iReg->iSTAT & (1<<4)))
        ILPC546xx_CAN_ISR(G_CAN0Workspace);

    if((G_CAN1Workspace) && (G_CAN1Workspace->iReg->iSTAT & (1<<4)))
        ILPC546xx_CAN_ISR(G_CAN1Workspace);

    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Interrupt Routine:  LPC546xx_CAN_Send_CAN
 *---------------------------------------------------------------------------*
 * Description:
 *
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_CAN_Send(
    void *aWorkspace,
    TUInt16 aID,
    TUInt8 *aData,
    TUInt8 aDataLen)
{
    T_LPC546xx_CAN_Workspace *aW = (T_LPC546xx_CAN_Workspace *)aWorkspace;
    T_LPC546xx_CAN_Registers *p = aW->iReg;

    if(aDataLen > 8)
        return UEZ_ERROR_INVALID_PARAMETER;

    p->iIF1_MCTRL = aDataLen & 0x0F;
    p->iIF1_DA1 = aData[0] + (aData[1]<<8);
    p->iIF1_DA2 = aData[2] + (aData[3]<<8);
    p->iIF1_DB1 = aData[4] + (aData[5]<<8);
    p->iIF1_DB2 = aData[6] + (aData[7]<<8);

    //Set to transmit
    p->iIF1_ARB2 = (1 << 15) | (1 << 13);
    p->iIF1_ARB1 = aID;

    p->iIF1_MSK2 = (1 << 14) & (0x7FFF < 2);
    p->iIF1_MSK1 = 0;

    p->iIF1_MCTRL |= (1 << 12) | (1 << 11) | (1 << 9) | (1 << 8) | (1 << 7) | (aDataLen & 0xF);

    p->iIF1_CMDMSK_W = (1 << 7) | 0x07;
    p->iIF1_CMDREQ = 1;

    while (p->iIF1_CMDREQ & 0x8000 ) {}

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Interrupt Routine:  LPC546xx_CAN_Enable_CAN
 *---------------------------------------------------------------------------*
 * Description:
 *
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_CAN_Activate(void *aWorkspace)
{
    T_LPC546xx_CAN_Workspace *aW = (T_LPC546xx_CAN_Workspace *)aWorkspace;
    T_LPC546xx_CAN_Registers *p = aW->iReg;

    p->iCNTL   |= (1<<1) | (1<<2); // Enable the Receive interrupt

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Interrupt Routine:  LPC546xx_CAN_Disable_CAN
 *---------------------------------------------------------------------------*
 * Description:
 *
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_CAN_Deactivate(void *aWorkspace)
{
    T_LPC546xx_CAN_Workspace *aW = (T_LPC546xx_CAN_Workspace *)aWorkspace;
    T_LPC546xx_CAN_Registers *p = aW->iReg;

    p->iCNTL   &= ~(1<<1); // Disable the Receive interrupt

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Interrupt Routine:  ILPC546xx_CAN_Interrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Catch CAN interrupts and use the ISR routine for it
 *---------------------------------------------------------------------------*/
static void ICANSetBaudRate(
    T_LPC546xx_CAN_Workspace *aW,
    TUInt32 aBaudRate)
{
    T_LPC546xx_CAN_Registers *p = aW->iReg;
    TUInt32 ClkDiv, BaudRatePrescaler, segs, Tseg1, Tseg2, clk_per_bit, SynJumpWidth;
    TBool success = EFalse;

    if(p->iCNTL & (1 << 0)){
        p->iCNTL |= (1 << 0);
    }

    clk_per_bit = PCLK_FREQUENCY / aBaudRate; // Hz

    for (ClkDiv = 0; ClkDiv <= 15; ClkDiv++) {
        for (BaudRatePrescaler = 1; BaudRatePrescaler <= 32; BaudRatePrescaler++) {
            for (segs = 3; segs <= 17; segs++) {
                if (clk_per_bit == (segs * BaudRatePrescaler * (ClkDiv + 1))) {
                    segs -= 3;
                    Tseg1 = segs / 2;
                    Tseg2 = segs - Tseg1;
                    SynJumpWidth = Tseg1 > 3 ? 3 : Tseg1;
                    success = ETrue;
                    break;
                }
            }
        }
    }

    if(success){
        /*Set bus timing */
        p->iCLKDIV = ClkDiv - 1;            /* Divider for CAN VPB3 clock */
        p->iCNTL |= (1<<6);                 /* Start configuring bit timing */
        p->iBT = (BaudRatePrescaler & 0x3F)
               | (SynJumpWidth & 0x03) << 6
               | (Tseg1 & 0x0F) << 8
               | (Tseg2 & 0x07) << 12;
        p->iBRPE = BaudRatePrescaler >> 6;  /* Set Baud Rate Prescaler MSBs */
        p->iCNTL &= ~(1<<6);                /* Stop configuring bit timing */

        /* Finish software initialization */
        p->iCNTL &= ~(1<<0);
        while ( p->iCNTL & (1<<0) ) {}
    }
}

/*---------------------------------------------------------------------------*
 * Interrupt Routine:  ICANSetupExplicitStandardAF
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup explicit standard address filter
 *---------------------------------------------------------------------------*/
static T_uezError ICANSetupExplicitStandardAF(
    T_LPC546xx_CAN_Workspace *aW,
    T_CAN_SFF_Entry *aSFFEntries,
    TUInt32 aNumEntries)
{
    // This routine was ported from the NXP lpc177x_8x_can.c driver
    // Please refer to it if we need to add other filter types
    // For now, we're only doing explicit standard AF for simplicity
    TUInt16 CANAF_std_cnt=0;
    TUInt8 ctrl1, dis1;
    TUInt16 SID, ID_temp,i, count = 0;
    TUInt32 entry;
    TUInt32 buf;

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
//                buf = LPC_CANAF_RAM->mask[count-1];
                ID_temp = (buf & 0xE7FF); //mask controller & identifier bits

                if (ID_temp > ((ctrl1<<13)|SID)) {
                    return UEZ_ERROR_OUT_OF_RANGE;
                }
            }

            entry = (ctrl1<<29)|(dis1<<28)|(SID<<16);

//            LPC_CANAF_RAM->mask[count] &= 0x0000FFFF;
//
//            LPC_CANAF_RAM->mask[count] |= entry;

            CANAF_std_cnt++;
            if (CANAF_std_cnt == aNumEntries)//if this is the last SFF entry
                count++;

        } else {
//            buf = LPC_CANAF_RAM->mask[count];
            ID_temp = (buf >>16) & 0xE7FF;

            if (ID_temp > ((ctrl1<<13)|SID)) {
                return UEZ_ERROR_OUT_OF_RANGE;
            }

            entry = (ctrl1 << 13) | (dis1 << 12) | (SID << 0);

//            LPC_CANAF_RAM->mask[count] &= 0xFFFF0000;
//
//            LPC_CANAF_RAM->mask[count] |= entry;

            count++;

            CANAF_std_cnt++;
        }
    }

    //update address values
//  LPC_CANAF->SFF_sa = 0;
//	LPC_CANAF->SFF_GRP_sa = LPC_CANAF->SFF_sa + (((CANAF_std_cnt+1)>>1)<< 2);
//	LPC_CANAF->EFF_sa = LPC_CANAF->SFF_GRP_sa;
//	LPC_CANAF->EFF_GRP_sa = LPC_CANAF->EFF_sa;
//	LPC_CANAF->ENDofTable = LPC_CANAF->EFF_GRP_sa;
//
//    LPC_CANAF->AFMR = 0x00; // Normal mode

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC546xx_CAN_SetCANSettings
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the CAN port settings
 * Inputs:
 *      void *aWorkspace          -- CAN port workspace
 *      T_CAN_Settings *aSettings -- Settings to use for CAN port
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_CAN_SetSettings(
        void *aWorkspace,
        T_CAN_Settings *aSettings)
{
    T_LPC546xx_CAN_Workspace *aW = (T_LPC546xx_CAN_Workspace *)aWorkspace;

    memcpy(&aW->iSettings, aSettings, sizeof(T_CAN_Settings));

    ICANSetBaudRate(aW, aSettings->iBaud);

    // need to setup acceptance filter here
    if(aW->iSettings.iNumSFFEntries > 0)
        ICANSetupExplicitStandardAF(aW, aW->iSettings.iSFFEntry, aW->iSettings.iNumSFFEntries);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC546xx_CAN_GetCANSettings
 *---------------------------------------------------------------------------*
 * Description:
 *      Return the previously configured CAN port
 * Inputs:
 *      void *aWorkspace          -- CAN port workspace
 *      T_CAN_Settings *aSettings -- Settings used for CAN port
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_CAN_GetSettings(
        void *aWorkspace,
        T_CAN_Settings *aSettings)
{
    T_LPC546xx_CAN_Workspace *aW = (T_LPC546xx_CAN_Workspace *)aWorkspace;

    // Record these settings
    *aSettings = aW->iSettings;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Interrupt Routine:  LPC546xx_CAN_Send_CAN
 *---------------------------------------------------------------------------*
 * Description:
 *
 *---------------------------------------------------------------------------*/
 T_uezError LPC546xx_CAN_Configure(
        void *aWorkspace,
        void *aCallbackWorkspace)
{
    T_LPC546xx_CAN_Workspace *aW = (T_LPC546xx_CAN_Workspace *)aWorkspace;
    T_LPC546xx_CAN_Registers *p = aW->iReg;

    p->iCNTL = 1;

	ICANSetBaudRate(aW, 1000000);   // Set baud rate to 1MHz - default

	p->iCNTL = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_CAN_CAN1_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1788 CAN workspace.
 * Inputs:
 *      void *aW                    -- Particular CAN workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_CAN_CAN0_InitializeWorkspace(void * aWorkspace)
{
    T_LPC546xx_CAN_Workspace *aW = (T_LPC546xx_CAN_Workspace *)aWorkspace;
    aW->iReg = (T_LPC546xx_CAN_Registers *)LPC_C_CAN0_BASE;
    G_CAN0Workspace = aW;

    aW->iCanNum = 0;

    LPC_CCU1->CLK_APB3_CAN0_CFG = 0x07;

    //Register the interrupt for this CAN but don't start it
    InterruptRegister(C_CAN0_IRQn, ILPC546xx_CAN_Interrupt,
            INTERRUPT_PRIORITY_NORMAL, "CAN");
    InterruptEnable(C_CAN0_IRQn);

    aW->iSettings.iReceiveCallback = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_CAN_CAN2_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1788 CAN workspace.
 * Inputs:
 *      void *aW                    -- Particular CAN workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_CAN_CAN1_InitializeWorkspace(void * aWorkspace)
{
    T_LPC546xx_CAN_Workspace *aW = (T_LPC546xx_CAN_Workspace *)aWorkspace;
    aW->iReg = (T_LPC546xx_CAN_Registers *)LPC_C_CAN1_BASE;
    G_CAN1Workspace = aW;

    aW->iCanNum = 1;

    LPC_CCU1->CLK_APB1_CAN1_CFG = 0x07;

    //Register the interrupt for this CAN but don't start it
    InterruptRegister(C_CAN1_IRQn, ILPC546xx_CAN_Interrupt,
            INTERRUPT_PRIORITY_NORMAL, "CAN");
    InterruptEnable(C_CAN1_IRQn);

    aW->iSettings.iReceiveCallback = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
 const HAL_CAN LPC546xx_CAN_CAN0_Interface = {
        {
                "CAN:LPC4357:CAN0",
                0x0100,
                LPC546xx_CAN_CAN0_InitializeWorkspace,
                sizeof(T_LPC546xx_CAN_Workspace),
        },

        LPC546xx_CAN_Configure,
        LPC546xx_CAN_SetSettings,
        LPC546xx_CAN_GetSettings,
        LPC546xx_CAN_Activate,
        LPC546xx_CAN_Deactivate,
        LPC546xx_CAN_Send,
};

const HAL_CAN LPC546xx_CAN_CAN1_Interface = {
        {
                "CAN:LPC4357:CAN1",
                0x0100,
                LPC546xx_CAN_CAN1_InitializeWorkspace,
                sizeof(T_LPC546xx_CAN_Workspace),
        },

        LPC546xx_CAN_Configure,
        LPC546xx_CAN_SetSettings,
        LPC546xx_CAN_GetSettings,
        LPC546xx_CAN_Activate,
        LPC546xx_CAN_Deactivate,
        LPC546xx_CAN_Send,
};

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC546xx_CAN0_Require(
    T_uezGPIOPortPin aPinRD,
    T_uezGPIOPortPin aPinTD)
{
    static const T_LPC546xx_SCU_ConfigList rd0[] = {
            {GPIO_P5_8,  SCU_NORMAL_DRIVE_DEFAULT(2)},
            {GPIO_P7_2,  SCU_NORMAL_DRIVE_DEFAULT(1)},
    };
    static const T_LPC546xx_SCU_ConfigList td0[] = {
            {GPIO_P5_9,  SCU_NORMAL_DRIVE_DEFAULT(2)},
            {GPIO_P7_3,  SCU_NORMAL_DRIVE_DEFAULT(1)},
    };

    HAL_DEVICE_REQUIRE_ONCE();

    HALInterfaceRegister("CAN0", (T_halInterface *)&LPC546xx_CAN_CAN0_Interface, 0, 0);

    LPC546xx_SCU_ConfigPinOrNone(aPinRD, rd0, ARRAY_COUNT(rd0));
    LPC546xx_SCU_ConfigPinOrNone(aPinTD, td0, ARRAY_COUNT(td0));
    LPC546xxPowerOn(1<<13);
}

void LPC546xx_CAN1_Require(
    T_uezGPIOPortPin aPinRD,
    T_uezGPIOPortPin aPinTD)
{
    static const T_LPC546xx_SCU_ConfigList rd1[] = {
            {GPIO_P0_13,  SCU_NORMAL_DRIVE_DEFAULT(5)},
            {GPIO_P5_13,  SCU_NORMAL_DRIVE_DEFAULT(6)},
            {GPIO_P7_1 ,  SCU_NORMAL_DRIVE_DEFAULT(5)},
    };
    static const T_LPC546xx_SCU_ConfigList td1[] = {
            {GPIO_P0_12,  SCU_NORMAL_DRIVE_DEFAULT(5)},
            {GPIO_P5_12,  SCU_NORMAL_DRIVE_DEFAULT(6)},
            {GPIO_P7_0 ,  SCU_NORMAL_DRIVE_DEFAULT(5)},
    };

    HAL_DEVICE_REQUIRE_ONCE();

    HALInterfaceRegister("CAN1", (T_halInterface *)&LPC546xx_CAN_CAN1_Interface, 0, 0);

    LPC546xx_SCU_ConfigPinOrNone(aPinRD, rd1, ARRAY_COUNT(rd1));
    LPC546xx_SCU_ConfigPinOrNone(aPinTD, td1, ARRAY_COUNT(td1));
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC546xx_CAN.c
 *-------------------------------------------------------------------------*/
