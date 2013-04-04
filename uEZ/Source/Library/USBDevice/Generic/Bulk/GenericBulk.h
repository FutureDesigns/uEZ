/*-------------------------------------------------------------------------*
 * File:  GenericBulk.h
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
#ifndef _GENERIC_BULK_H_
#define _GENERIC_BULK_H_

#ifndef GENERIC_BULK_QUEUE_IN_SIZE
#define GENERIC_BULK_QUEUE_IN_SIZE  256
#endif

#ifndef GENERIC_BULK_QUEUE_OUT_SIZE
#define GENERIC_BULK_QUEUE_OUT_SIZE 2048
#endif

typedef struct {
    void *iCallbackWorkspace;
    void (*iGenBulkEmptyOutput)(void *aCallbackWorkspace);
} T_GenBulkCallbacks;

// Main Virtual Comm Functions:
T_uezError GenericBulkInitialize(T_GenBulkCallbacks *aCallbacks);
TBool GenericBulkPut(TUInt8 c, TUInt32 aTimeout);
TInt32 GenericBulkGet(TUInt32 aTimeout);

#endif // _GENERIC_BULK_H_

/*-------------------------------------------------------------------------*
 * End of File:  GenericBulk.h
 *-------------------------------------------------------------------------*/
