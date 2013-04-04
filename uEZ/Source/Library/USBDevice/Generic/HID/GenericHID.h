/*-------------------------------------------------------------------------*
 * File:  GenericHID.h
 *-------------------------------------------------------------------------*
 | Description:
 |      USB implementation of the device side of a virtual comm port
 |      following CDC rules.
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
#ifndef _GENERIC_HID_H_
#define _GENERIC_HID_H_

#ifndef GENERIC_HID_QUEUE_IN_SIZE
#define GENERIC_HID_QUEUE_IN_SIZE  256
#endif

#ifndef GENERIC_HID_QUEUE_OUT_SIZE
#define GENERIC_HID_QUEUE_OUT_SIZE 256
#endif

typedef struct {
    void (*iGHIDEmptyOutput)(void);
} T_GenHIDCallbacks;

// Main Virtual Comm Functions:
T_uezError GenericHIDInitialize(T_GenHIDCallbacks *aCallbacks);
TBool GenericHIDPut(TUInt8 c, TUInt32 aTimeout);
TInt32 GenericHIDGet(TUInt32 aTimeout);

#endif // _GENERIC_HID_H_

/*-------------------------------------------------------------------------*
 * End of File:  GenericHID.h
 *-------------------------------------------------------------------------*/
