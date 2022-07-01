/*-------------------------------------------------------------------------*
 * File:  USBHID_Mouse_Task.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
 
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <uEZ.h>
#include <uEZPlatform.h>
#include "USBHID_Mouse_Task.h"

static void MouseTask(void)
{
    T_uezError error;
    T_HIDMouseReport report;
    int x=0, y=0, w=0;

    while (1) {
        error = HIDMouseRead(&report, UEZ_TIMEOUT_INFINITE);
        if (error == UEZ_ERROR_NONE) {
              x += report.iDeltaX;
              y += report.iDeltaY;
              w += report.iDeltaWheel;
            printf("X: %4d, Y: %4d, W: %4d, buttons: %s %s %s %s %s\n", x, y, w,
                    ((report.iButtons & MOUSE_BUTTON_LEFT) ? "LEFT" : ""),
                    ((report.iButtons & MOUSE_BUTTON_RIGHT) ? "RIGHT" : ""),
                    ((report.iButtons & MOUSE_BUTTON_MIDDLE) ? "MIDDLE" : ""),
                    ((report.iButtons & MOUSE_BUTTON_FORWARD) ? "FORWARD" : ""),
                    ((report.iButtons & MOUSE_BUTTON_BACK) ? "BACK" : "")
                    );
        }
    }
}

void USBHID_Mouse_Start()
{
	static TBool once = EFalse;
	
	if(!once){
	    once = ETrue;
		
		UEZTaskCreate((T_uezTaskFunction)MouseTask, "Mouse", MAIN_TASK_STACK_SIZE, 0,
                  UEZ_PRIORITY_NORMAL, NULL);
	}
}
/*-------------------------------------------------------------------------*
 * End of File:  USBHID_Mouse_Task.c
 *-------------------------------------------------------------------------*/