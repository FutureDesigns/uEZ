/*HEADER**********************************************************************
Copyright (c)
All rights reserved
This software embodies materials and concepts that are confidential to Redpine
Signals and is made available solely pursuant to the terms of a written license
agreement with Redpine Signals

Project name : 
Module name  :
File Name    :process_data.c

File Description:
 
List of functions:
    

Author :

Rev History:
Ver   By               date        Description
---------------------------------------------------------
1.1  Redpine Signals
---------------------------------------------------------
*/

#include "includes.h"
#include "process_data.h"
//TBD: #include "leds.h"
//TBD: #include "iodefine.h"

/*FUNCTION*********************************************************************
Function Name  : rsi_process_data
Description    : This function processes the remote peer LED request related data
Returned Value : NONE
Parameters     : 

-----------------+-----+-----+-----+------------------------------
Name             | I/P | O/P | I/O | Purpose
-----------------+-----+-----+-----+------------------------------
rx_buf           | X   |     |     |  Remote peer LED request buffer
*END****************************************************************************/
#include <stdio.h>
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZLEDBank.h>
#include <uEZButton.h>
#include <Device/LEDBank.h>
#include <Device/ButtonBank.h>
int32_t rsi_process_data(unsigned char *rx_buff)
{
#if 1 // new version for DK
        //uint8_t LED_START_PIN = 3;
        static int G_ledIsOn[8];
	int i;
        static T_uezDevice ledBank = 0;
        T_uezError error;

  if (!ledBank) {
    DEVICE_LEDBank **p_leds;
    T_uezDevice leds;
    DEVICE_ButtonBank **p_buttons;
    T_uezDevice buttons;

        error = UEZDeviceTableFind("LEDBank0", &leds);
        if (error)
            return error;
        error = UEZDeviceTableGetWorkspace(leds, (T_uezDeviceWorkspace **)&p_leds);
        if (error)
            return error;
        
        error = UEZDeviceTableFind("ButtonBank0", &buttons);
        if (error)
            return error;
        error = UEZDeviceTableGetWorkspace(buttons, (T_uezDeviceWorkspace **)&p_buttons);
        if (error)
            return error;
        
        (*p_leds)->SetBlinkRegister(p_leds, 0, 500, 128);
        (*p_leds)->SetBlinkRegister(p_leds, 1, 250, 64);
        
        (*p_buttons)->SetActiveButtons(p_buttons, 0x0F);

      UEZLEDBankOpen("LEDBank0", &ledBank);
      for (i=0; i<3; i++)
        UEZLEDOn(ledBank, i);
  }
  if (!ledBank)
    return RSI_STATUS_FAIL;

    if (!((rx_buff[0] != LED) || (rx_buff[1] > SET_REQ) || (rx_buff[2] > 2)))
   {
	   if(rx_buff[2] == 0)
	   {
		  //LED_START_PIN = 5; 
	   }
	   else if(rx_buff[2] == 2)
	   {
		 //LED_START_PIN = 1;
	   }
       if (rx_buff[1] == SET_REQ)
       {
           if (rx_buff[3] == 1) /* switch off LED */
           {
//               LED_ADDR |= (0x1 << (LED_START_PIN  + rx_buff[2]));    
              printf("LED off %d\n", rx_buff[2]);
              G_ledIsOn[rx_buff[2]] = 1;
              UEZLEDOff(ledBank, 4+rx_buff[2]);
			    
		   }
           else if (rx_buff[3] == 0) /* switch on LED */
           {
//               LED_ADDR &= (~(0x1 << (LED_START_PIN  + rx_buff[2])));   
              printf("LED on %d\n", rx_buff[2]);
              G_ledIsOn[rx_buff[2]] = 0;
              UEZLEDOn(ledBank, 4+rx_buff[2]);
			
           }        
       }             
//	   rx_buff[3] = (!!(LED_ADDR  & (0x1 << (LED_START_PIN  + rx_buff[2]))));	
//          rx_buff[3] = 0;
//          for (i=0; i<8; i++)
//              if (G_ledIsOn[i])
//                  rx_buff[3] |= (1<<i);
          rx_buff[3] = G_ledIsOn[rx_buff[2]];
	   rx_buff[1] += 2;
	   return RSI_STATUS_SUCCESS;
   }
#endif
#if 0 // TBD:
        uint8_t LED_START_PIN = 3;
	
    if (!((rx_buff[0] != LED) || (rx_buff[1] > SET_REQ) || (rx_buff[2] > 2)))
   {
	   if(rx_buff[2] == 0)
	   {
		  LED_START_PIN = 5; 
	   }
	   else if(rx_buff[2] == 2)
	   {
		 LED_START_PIN = 1;
	   }
       if (rx_buff[1] == SET_REQ)
       {
           if (rx_buff[3] == 1) /* switch off LED */
           {
               LED_ADDR |= (0x1 << (LED_START_PIN  + rx_buff[2]));    
			    
		   }
           else if (rx_buff[3] == 0) /* switch on LED */
           {
               LED_ADDR &= (~(0x1 << (LED_START_PIN  + rx_buff[2])));   
			
           }        
       }             
	   rx_buff[3] = (!!(LED_ADDR  & (0x1 << (LED_START_PIN  + rx_buff[2]))));	 
	   rx_buff[1] += 2;
	   return RSI_STATUS_SUCCESS;
   }
#endif
   return RSI_STATUS_FAIL;
}