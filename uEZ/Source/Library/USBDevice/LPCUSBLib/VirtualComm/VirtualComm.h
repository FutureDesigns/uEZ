/*-------------------------------------------------------------------------*
* File:  VirtualComm.h
*--------------------------------------------------------------------------*
* Description:
*      USB implementation of the device side of a virtual comm port
*      following CDC rules.
*-------------------------------------------------------------------------*/
#ifndef _VIRTUAL_COMM_H_
#define _VIRTUAL_COMM_H_

/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://goo.gl/UDtTCR for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif


#ifndef VIRTUAL_COMM_QUEUE_IN_SIZE
#define VIRTUAL_COMM_QUEUE_IN_SIZE  1024
#endif

#ifndef VIRTUAL_COMM_QUEUE_OUT_SIZE
#define VIRTUAL_COMM_QUEUE_OUT_SIZE 1024
#endif

typedef struct {
    void (*iVCLineSpeedChange)(TUInt32 aSpeed);
    void (*iVCControlLineState)(TBool aRTS, TBool aDTR);
    void (*iVCEmptyOutput)(void); // NOT USED YET
} T_vcommCallbacks;

// Main Virtual Comm Functions:
T_uezError VirtualCommInitialize(T_vcommCallbacks *aCallbacks, int32_t aUnitAddress, TUInt8 aForceFullSpeed);
TBool VirtualCommPut(TUInt8 c, TUInt32 aTimeout);
TInt32 VirtualCommGet(TUInt32 aTimeout);

#ifdef __cplusplus
}
#endif

#endif // _VIRTUAL_COMM_H_

/*-------------------------------------------------------------------------*
 * End of File:  VirtualComm.h
 *-------------------------------------------------------------------------*/
