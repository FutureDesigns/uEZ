/*-------------------------------------------------------------------------*
 * File: MyTask.c
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * uEZ(R) - Copyright (C) 2007-2016 Future Designs, Inc.
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZTemperature.h>
#include <uEZGPIO.h>
#include <uEZProcessor.h>
#include "MyTask.h"
#include <stdio.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/   
#define LEDONTIME       100 // Time the LED is lit (mS)
#define LEDOFFTIME      850 // Time the LED is off (mS)
#define LEDPIN          GPIO_P0_11 // GPIO pin the on-board LED is on
   
/*-------------------------------------------------------------------------*
 * Externs:
 *-------------------------------------------------------------------------*/
extern void UpdateTemp(char *myString); // Access to the update function in SecondScreen.c

/*-------------------------------------------------------------------------*
 * Routine:    TemperatureLoopTask
 *-------------------------------------------------------------------------*
 * Description:
 *      Task to take temperature reading and update screen
 *-------------------------------------------------------------------------*/
TUInt32 TemperatureLoopTask(T_uezTask aMyTask, void *aParams)
{
    char myString[10] = ""; // String to hold temperature

    TInt32 i, f; // Variables for conversion

    T_uezDevice temp; // uEZ device variable
    TInt32 Temperature; // Variable to hold temperature reading

    UEZTemperatureOpen("Temp0", &temp); // Open the on board device

    while(1) {
        UEZTemperatureRead(temp, &Temperature); // Read the temperature

        i = Temperature >> 16;
        f = ((((TUInt32)Temperature) & 0xFFFF) * 10) >> 16;  // Convert to 1 digit decimal
        sprintf(myString, "%02d.%01d *C", i, f); // Print to temperature text array

        UpdateTemp(myString); // Update the temperature text on the second screen
        UEZTaskDelay(250); // Wait 1/4 seconds before reading again
    }
}

/*-------------------------------------------------------------------------*
 * Routine:    HeartbeatTask
 *-------------------------------------------------------------------------*
 * Description:
 *      Task to blink the on board LED
 *-------------------------------------------------------------------------*/
TUInt32 HeartbeatTask(T_uezTask aMyTask, void *aParams)
{
    UEZGPIOSetMux(LEDPIN, 0); // Set the Mux
    UEZGPIOOutput(LEDPIN); // Set the pin to output

    while(1) {
        UEZGPIOSet(LEDPIN); // On Board LED High
        UEZTaskDelay(LEDONTIME); // Delay
        UEZGPIOClear(LEDPIN); // On Board LED Low
        UEZTaskDelay(LEDOFFTIME); // Delay
    }
}

/*-------------------------------------------------------------------------*
 * End of File:    MyTask.c
 *-------------------------------------------------------------------------*/