/*
 ***************************************************************************
 *  Module Name: vCANTransceive
 *     Filename: CANTransceive.c
 *       Author: Jim Harris
 *     Revision: 1.0
 *    Rev. Date: September 12,2008
 *    Mod. Time: 11:22
 *
 *  Description:
 *    This code will perform all the necessary housekeeping to
 *    transmit and receive CAN messages. Format for transmission
 *    is always 8 bytes.
 *
 *  Rev. History:
 *    Rev 0.2   August 18,2008 jharris
 *      Now set to work with queues for 7-Segement
 *    Rev 1.0   September 12, 2008
 *       Cleaned up all comments/look-and-feel for release
 *       Added standard disclaimer
 *   Greatly mangled for this test -- LShields 2/18/2009
 ***************************************************************************
*/

/*
********************************************************************************
*
* Software that is described herein is for illustrative purposes only which
* provides customers with programming information regarding the products.
* This software is supplied "AS IS" without any warranties. NXP Semiconductors
* assumes no responsibility or liability for the use of the software, conveys
* no license or title under any patent, copyright, or mask work right to the
* product. NXP Semiconductors reserves the right to make changes in the
* software without notification. NXP Semiconductors also make no representation
* or warranty that such application will be suitable for the specified use
* without further testing or modification.
********************************************************************************
*/

/* Standard includes */
#include <stdlib.h>
#include <uEZ.h>
#include <uEZProcessor.h>
#include <uEZGPIO.h>

/* Scheduler includes */
#include "FreeRTOS.h"
#include "task.h"

/* Header file for using queues etc  */
#include "queue.h"

/* Local includes  */
//#include "bsp\LPC2119startup.h"

/* Variables used for building CAN message buffer */
//extern int ADCdataR38;  /* Value digitized from R38 */
//extern int ADCdataR37;  /* Value digitized from R37 */
//extern int ADCdataR36;  /* Value digitized from R37 */
//extern int ButtonsRead; /* Value of pushbuttons pressed, or not */
//extern int ButtonHistory1;  /* Status of pushbutton 1 history */
int CANRCV;
//extern unsigned int R_Temp; /* Remote temperature reading */
//extern unsigned int L_Temp; /* Local temperature reading */

/* Scheduler defines   */
#define canSTACK_SIZE                configMINIMAL_STACK_SIZE /*100 bytes*/

/*note that there are "port" (FreeRTOS) specific labelling needs*/
extern portTASK_FUNCTION_PROTO( vCANmsg, pvParameters );

unsigned int CANbyteA,CANbyteB;
long CANTXByte1,CANTXByte2;

/*local function definitions  */
extern void vCANTransceive (void);
void BufferBuild ( void ) ;

int G_canConfigured = 0;
int G_forceCANTransmit = 0;

void CANConfigure ( void )
{
#if (UEZ_PROCESSOR==NXP_LPC2478)
        UEZGPIOSetMux(GPIO_P0_0, 1);
        UEZGPIOSetMux(GPIO_P0_1, 1);
//    unsigned int n = ((Fcclk/4)/125000)-1;
    PCONP |= PCONP_PCAN1;
    PCLKSEL0 |= (2<<PCLKSEL0_PCLK_CAN1_BIT)|(2<<PCLKSEL0_PCLK_ACF_BIT);
        C1MOD        =        0x00000001;        /* Set CAN controller into reset */
        C1BTR        =        0x001C0011;        /* Set bit timing to 125k -- old value from 2119 0x1D */
        C1IER        =        0x00000000;        /* Disable the Receive interrupt */
        AFMR        =        0x00000002;        /* Bypass acceptance filters to receive all CAN traffic */
        C1TFI1        =        0x00080000;        /* Set DLC to transmit 8 bytes */
        C1TID1        =        0x00000555;        /* Set CAN ID to '555' */
#if 1
        C1MOD        =        0x00000000;        /* Release CAN controller */
#endif
#endif
#if (UEZ_PROCESSOR==NXP_LPC1788)
        UEZGPIOSetMux(GPIO_P0_0, 1);
        UEZGPIOSetMux(GPIO_P0_1, 1);
//    unsigned int n = ((Fcclk/4)/125000)-1;
    //PCONP |= PCONP_PCAN1;
    LPC17xx_40xxPowerOn(1<<13); // CAN1 power on
    //PCLKSEL0 |= (2<<PCLKSEL0_PCLK_CAN1_BIT)|(2<<PCLKSEL0_PCLK_ACF_BIT);
    LPC_CAN1->MOD        =        0x00000001;        /* Set CAN controller into reset */
    LPC_CAN1->BTR        =        0x001C0000|(30-1);        /* Set bit timing to 125k -- old value from 2119 0x1D */
    LPC_CAN1->IER        =        0x00000000;        /* Disable the Receive interrupt */
    LPC_CANAF->AFMR      =        0x00000002;        /* Bypass acceptance filters to receive all CAN traffic */
    LPC_CAN1->TFI1        =        0x00080000;        /* Set DLC to transmit 8 bytes */
    LPC_CAN1->TID1        =        0x00000555;        /* Set CAN ID to '555' */
#if 1
    LPC_CAN1->MOD        =        0x00000000;        /* Release CAN controller */
#endif
#endif
        G_canConfigured = 1;
}

int CANReceive8(unsigned int *aCANByteA, unsigned int *aCANByteB)
{
#if(UEZ_PROCESSOR != NXP_LPC4357)
    unsigned CANrdy = 0;
#endif

    if (!G_canConfigured)
        CANConfigure();

#if (UEZ_PROCESSOR==NXP_LPC2478)
    {
        CANrdy = C1GSR&0x00000001;        /* Get status of receiver buffer */
        if (CANrdy != 0) {
          /* We have received a CAN message, so we need to process it */
          CANRCV = pdTRUE;
          *aCANByteA = C1RDA; /* Get the first 4 bytes of the received CAN message */
          *aCANByteB = C1RDB; /* Get the last 4 bytes of the received CAN message */

          C1CMR =   0x00000004; /* Release the receive buffer */
         }

    }
    return (CANrdy)?1:0;
#elif (UEZ_PROCESSOR==NXP_LPC1788)
    {
        CANrdy = LPC_CAN1->GSR&0x00000001;        /* Get status of receiver buffer */
        if (CANrdy != 0) {
          /* We have received a CAN message, so we need to process it */
          CANRCV = pdTRUE;
          *aCANByteA = LPC_CAN1->RDA; /* Get the first 4 bytes of the received CAN message */
          *aCANByteB = LPC_CAN1->RDB; /* Get the last 4 bytes of the received CAN message */

          LPC_CAN1->CMR =   0x00000004; /* Release the receive buffer */
         }

    }
    return (CANrdy)?1:0;
#elif (UEZ_PROCESSOR==NXP_LPC4088)
    {
        CANrdy = LPC_CAN1->GSR&0x00000001;        /* Get status of receiver buffer */
        if (CANrdy != 0) {
          /* We have received a CAN message, so we need to process it */
          CANRCV = pdTRUE;
          *aCANByteA = LPC_CAN1->RDA; /* Get the first 4 bytes of the received CAN message */
          *aCANByteB = LPC_CAN1->RDB; /* Get the last 4 bytes of the received CAN message */

          LPC_CAN1->CMR =   0x00000004; /* Release the receive buffer */
         }

    }
    return (CANrdy)?1:0;
#else
  return 0;
#endif //(UEZ_PROCESSOR==NXP_LPC2478)
}/* vCANmsg */


/*
 ***************************************************************************
 *  Module Name: BufferBuild
 *     Filename: CANTransceive.c
 *       Author: Jim Harris
 *     Revision: 1.0
 *    Rev. Date: September 12, 2008
 *    Mod. Time: 14:33
 *
 * Requirements:
 *
 *  Description: This module will build the CAN buffer from the various
 *                pieces of data collected.
 *
 * Rev. History:
 *    Rev 0.0   August 21,2008 jharris
 *        Initial code written
 *    Rev 1.0
 *        Cleaned up all comments/look-and-feel for release
 *        Added standard disclaimer
 ***************************************************************************
*/
void  CANSend8(unsigned int CANTXByte1, unsigned int CANTXByte2)
  {
#if (UEZ_PROCESSOR==NXP_LPC2478)
    /* Let's build up the CAN message buffer */
    if (!G_canConfigured)
        CANConfigure();

#if 0
          /* First, we build up the first long word, which will be CAN message bytes 1-4 */
    CANTXByte1 = 0x00; // R_Temp;  /* First byte is the Remote Temperature Reading */
    CANTXByte1 = CANTXByte1 | (/*L_Temp*/0x11 << 8);  /* Second byte is the Local Temperature Reading */
    CANTXByte1 = CANTXByte1 | (/*ADCdataR36*/0x22 << 16); /* Third byte is the reading of R36 */
    CANTXByte1 = CANTXByte1 | (/*ADCdataR37*/0x33 << 24); /* Fourth byte is the reading of R37 */

        /* Next, we build up the second long word, which will be CAN message bytes 5-8 */
    CANTXByte2 = 0x00; // ADCdataR38;  /* First byte is the reading of R38 */
    CANTXByte2 = CANTXByte2 | (0x00 /*ButtonsRead*/ << 8); /* Second byte is the status of the 4 pushbuttons */
    CANTXByte2 = CANTXByte2 | (0x00 << 16); /* Third byte is for LED status...from CAN board, is NULL */
    CANTXByte2 = CANTXByte2 | (0x00 << 24); /* Fourth byte is for Piezo Frequency...from CAN board, is NULL */
#endif

          C1TDA1  =      CANTXByte1;      /* First 4 bytes of the CAN message */
          C1TDB1  =      CANTXByte2;      /* Next 4 bytes of the CAN Message */
          C1CMR        =      0x00000021;      /* Transmit the message */
#endif // (UEZ_PROCESSOR==NXP_LPC2478)
#if (UEZ_PROCESSOR==NXP_LPC1788)
    /* Let's build up the CAN message buffer */
    if (!G_canConfigured)
        CANConfigure();

          LPC_CAN1->TDA1  =      CANTXByte1;      /* First 4 bytes of the CAN message */
          LPC_CAN1->TDB1  =      CANTXByte2;      /* Next 4 bytes of the CAN Message */
          LPC_CAN1->CMR        =      0x00000021;      /* Transmit the message */
#endif // (UEZ_PROCESSOR==NXP_LPC2478)
  }/* BufferBuild */
