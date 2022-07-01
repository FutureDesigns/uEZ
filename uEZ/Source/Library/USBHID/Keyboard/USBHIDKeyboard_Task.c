/*-------------------------------------------------------------------------*
 * File:  USBHIDKeyboard_Task.c
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
 
#include <string.h>
#include <stdio.h>
#include <uEZ.h>
#include <uEZPlatform.h>
#include "USBHIDKeyboard_Task.h"

// Example keyboard task:
static void KeyboardTask(void)
{
    T_uezError error;
    T_HIDKeyboardReport keyscan;
    int32_t i;
    TUInt8 state = 0;
    const TUInt8 pattern[4] = { 0x01, 0x02, 0x04, 0x02 };

    while (1) {
        error = HIDKeyboardRead(&keyscan, UEZ_TIMEOUT_INFINITE);
        //printf("  Keyboard Task: Got key: %d\n", key);
        if (error == UEZ_ERROR_NONE) {
            printf("\nGot a keyboard reading:\n");
            if (keyscan.iFlags & KEYBOARD_SCAN_FLAG_LEFT_CONTROL)
                printf("  Left CTRL\n");
            if (keyscan.iFlags & KEYBOARD_SCAN_FLAG_LEFT_SHIFT)
                printf("  Left SHIFT\n");
            if (keyscan.iFlags & KEYBOARD_SCAN_FLAG_LEFT_ALT)
                printf("  Left ALT\n");
            if (keyscan.iFlags & KEYBOARD_SCAN_FLAG_LEFT_GUI)
                printf("  Left GUI\n");
            if (keyscan.iFlags & KEYBOARD_SCAN_FLAG_RIGHT_CONTROL)
                printf("  Right CTRL\n");
            if (keyscan.iFlags & KEYBOARD_SCAN_FLAG_RIGHT_SHIFT)
                printf("  Right SHIFT\n");
            if (keyscan.iFlags & KEYBOARD_SCAN_FLAG_RIGHT_ALT)
                printf("  Right ALT\n");
            if (keyscan.iFlags & KEYBOARD_SCAN_FLAG_RIGHT_GUI)
                printf("  Right GUI\n");

            for (i=0; i<6; i++) {
                if (keyscan.iKeys[i]) {
                    printf("  Pressing [%s]\n", HIDKeyboardScanKeyAsText(keyscan.iKeys[i]));
                }
            }

            // Pressing any keys?
            if (keyscan.iKeys[0]) {
                // Cycle around to the next pattern
                state++;
                if (state >= 4)
                    state = 0;
                HIDKeyboardSetIndicators(pattern[state]);
            }
        }
    }
}

void USBHIDKeyboard_Start()
{
	static TBool once = EFalse;
	
	if(!once){
	    once = ETrue;
				
		UEZTaskCreate((T_uezTaskFunction)KeyboardTask, "Keyboard", MAIN_TASK_STACK_SIZE, 0,
					  UEZ_PRIORITY_NORMAL, NULL);
	}
}
/*-------------------------------------------------------------------------*
 * End of File:  USBHIDKeyboard_Task.c
 *-------------------------------------------------------------------------*/