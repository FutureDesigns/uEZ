/*-------------------------------------------------------------------------*
 * File:  uEZTS.c
 *-------------------------------------------------------------------------*
 * Description:
 *      UEZlex Touchscreen Subsystem and API.
 *
 * Implementation:
 *      The Touchscreen subsystem is more than just a thin API to
 *      the touchscreen device(s).  It also provides a task that polls
 *      the touchscreen(s) and generates events placed on queues.
 *
 *      A list of all open touchscreen devices is kept along with a list
 *      each for queues receiving events.  When a touchscreen event
 *      occurs, it is sent to all receivers.
 *
 *      When an event is detected, the event is only reported if the queue
 *      has room, otherwise, the state change is held.
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
/**
 *    @addtogroup uEZTS
 *  @{
 *  @brief     uEZ Touchscreen Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZLicense.txt
 *
 *    The uEZ Touchscreen interface.
 *
 *  @par Example code:
 *  Example task to setup the touchscreen
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTS.h>
 *
 *  TUInt32 setupTouchscreen(T_uezTask aMyTask, void *aParams)
 *  {
 *      T_uezDevice TS;
 *      T_uezQueue aTSQueue;
 *      T_uezTSReading aReading;
 *
 *      // Setup a queue to receive touch screen events
 *      if (UEZQueueCreate(1, sizeof(T_uezTSReading), &aTSQueue) != UEZ_ERROR_NONE) {
 *          EPRINTF("Could not allocate queue for touchscreen\n");
 *          return 1;
 *      }
 *      if (UEZTSOpen("Touchscreen", &TS, &aTSQueue) == UEZ_ERROR_NONE) {
 *	 
 *          UEZTSGetReading(TS, &aReading);
 *          //  Touchscreen data now in aReading structure
 *
 *          if (UEZTSClose(TS, aTSQueue) != UEZ_ERROR_NONE) {
 *              // error closing the device
 *          }
 *
 *      } else {
 *          // an error occurred opening the device
 *      }
 *      return 0;
 *  }
 *  @endcode
 */
#include "Config.h"
#include <uEZTS.h>
#include <HAL/HAL.h>
#include <Device/Touchscreen.h>
#include <uEZDevice.h>
#include <uEZDeviceTable.h>
#include <uEZMemory.h>
#include <uEZLCD.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct tag_tsQueue {
    struct tag_tsQueue *iNext;
    T_uezQueue iQueue;
} T_tsQueue;

typedef struct tag_tsDevice {
    struct tag_tsDevice *iNext;
    T_uezDevice iDevice;
    T_tsQueue *iQueueList;
} T_tsDevice;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
// Track if we have initialized the global data
static TBool G_tsDidInit = EFalse;

// List of touchscreen devices
static T_tsDevice *G_tsDevices = 0;

// Semaphore for thread synchronization
static T_uezSemaphore G_tsSem;

// Task handle of touchscreen monitoring task
static T_uezTask G_tsMonitorTask;

// Declare static TS queues when not using dynamic memory allocation. 
#ifdef NO_DYNAMIC_MEMORY_ALLOC
#define ueztsMAX_TS_QUEUES			( 5 )
static T_tsQueue xTsQueues[ ueztsMAX_TS_QUEUES ];
static T_tsQueue *xFreeTsList;
#endif  // NO_DYNAMIC_MEMORY_ALLOC

static TBool G_WaitForRelease = EFalse;

/*---------------------------------------------------------------------------*
 * Internal prototypes:
 *---------------------------------------------------------------------------*/
static void IUEZTSGrab(void);
static void IUEZTSRelease(void);

/*---------------------------------------------------------------------------*
 * Task:  IUEZTSMonitorTouchscreensTask
 *---------------------------------------------------------------------------*/
/**
 *  The monitor touchscreens routine is used to monitor all touchscreens,
 *      poll their settings, and report events.
 *
 *  @param [in]    aMyTask       	This task's handle
 *
 *  @param [in]    *aParams     	Parameters for this task (not used)
 *
 *  @return        TUInt32        	Result code.  (not used)
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTS.h>
 *
 *  // see IUEZTSInitialize
 *  UEZTaskCreate(
 *      (T_uezTaskFunction)IUEZTSMonitorTouchscreensTask,
 *      "TS_Mon",
 *      UEZ_TASK_STACK_BYTES( 512 ), // Might need to be 1024 for uEZGUI
 *      0,
 *      UEZ_PRIORITY_HIGH,
 *      &G_tsMonitorTask);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void IUEZTSMonitorTouchscreensTask(T_uezTask aMyTask, void *aParams)
{
    T_tsDevice *p;
    T_tsQueue *pq;
    T_uezTSReading reading;

    for (;;) {
        IUEZTSGrab();
        p = G_tsDevices;
        while (p) {
            if (UEZTSGetReading(p->iDevice, &reading) == UEZ_ERROR_NONE)  {
            
                if(reading.iFlags & TSFLAG_PEN_DOWN) {
                    UEZLCDScreensaverWake();
                    
                    if(UEZLCDScreensaverIsActive())
                        G_WaitForRelease = ETrue;
                }
                
                if(G_WaitForRelease) {
                    if(!(reading.iFlags & TSFLAG_PEN_DOWN)) {
                        // if the pen is released, stop waiting
                        G_WaitForRelease = EFalse;
                    }
                } else {
                    // Apply the reading to all the queues (as last reported)
                    for (pq = p->iQueueList; pq; pq=pq->iNext) {
                        // Send it on the queue.
                        UEZQueueSend(pq->iQueue, &reading, 0);
                    }
                }
            }
            p = p->iNext;
        }
        IUEZTSRelease();
        UEZTaskDelay(10);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  IUEZTSInitialize
 *---------------------------------------------------------------------------*/
/**
 *  Initialize the touchscreen subsystem.  Initiliaze the list of devices
 *      and the touchscreen monitoring task.
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTS.h>
 *
 *  UEZTSInitialize(); // used in UEZTSOpen
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void IUEZTSInitialize(void)
{
    G_tsDevices = 0;
    if (UEZSemaphoreCreateBinary(&G_tsSem) == UEZ_ERROR_NONE)
        G_tsDidInit = ETrue;

    UEZTaskCreate(
        (T_uezTaskFunction)IUEZTSMonitorTouchscreensTask,
        "TS_Mon",
        UEZ_TASK_STACK_BYTES( 512 ), // Might need to be 1024 for uEZGUI
        0,
        UEZ_PRIORITY_HIGH,
        &G_tsMonitorTask);
	
#ifdef NO_DYNAMIC_MEMORY_ALLOC
	{
	TUInt16 usLoop;
	
		/* initialise the TS queue list. */
		for( usLoop = 0; usLoop < ( ueztsMAX_TS_QUEUES - 1 ); usLoop++ )
		{
			xTsQueues[ usLoop ].iNext = &xTsQueues[ usLoop + 1 ];  	
		}
		xTsQueues[ usLoop ].iNext = NULL;
		xFreeTsList = &xTsQueues[ 0 ];
	}
#endif  // NO_DYNAMIC_MEMORY_ALLOC	
}
/*---------------------------------------------------------------------------*
 * Routine:  UEZTSOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open a touchscreen device and register queue (if given)
 *
 *  @param [in]    *aName   		Name of touchscreen device
 *
 *  @param [in]    *aDevice   		Opened device (if opened)
 *
 *  @param [in]    *aEventQueue     Pointer to queue to receive touchscreen 
 *                                  events.
 *  @return        T_uezError       Error code, or UEZ_ERROR_NONE if opened.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTS.h>
 *
 *  T_uezDevice TS;
 *  T_uezQueue aTSQueue;
 *  // Setup a queue to receive touch screen events
 *  if (UEZQueueCreate(1, sizeof(T_uezTSReading), &aTSQueue) != UEZ_ERROR_NONE) {
 *      EPRINTF("Could not allocate queue for touchscreen\n");
 *  } else {
 *
 *      if (UEZTSOpen("Touchscreen", &TS, &aTSQueue) == UEZ_ERROR_NONE) {
 *          // the device opened properly
 *      } else {
 *          // an error occurred opening the device
 *      }
 *
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTSOpen(
            const char * const aName,
            T_uezDevice *aDevice,
            T_uezQueue *aEventQueue)
{
    T_uezError error;
    DEVICE_TOUCHSCREEN **p;

    if (!G_tsDidInit)
        IUEZTSInitialize();

    error = UEZDeviceTableFind(aName, aDevice);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(*aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    error = (*p)->Open((void *)p);
    if (error)
        return error;

    if (aEventQueue)
        UEZTSAddQueue(*aDevice, *aEventQueue);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZTSClose
 *---------------------------------------------------------------------------*/
/**
 *  Close a previously opened touchscreen device.
 *
 *  @param [in]    aDevice       	Touchscreen device to close
 *
 *  @param [in]    aTSQueue       	Queue being used with touchscreen
 *
 *  @return        T_uezError       Error code, or UEZ_ERROR_NONE if closed
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTS.h>
 *
 *  T_uezDevice TS;
 *  T_uezQueue aTSQueue;
 *  // Setup a queue to receive touch screen events
 *  if (UEZQueueCreate(1, sizeof(T_uezTSReading), &aTSQueue) != UEZ_ERROR_NONE) {
 *      EPRINTF("Could not allocate queue for touchscreen\n");
 *  } else {
 *      if (UEZTSOpen("Touchscreen", &TS, &aTSQueue) == UEZ_ERROR_NONE) {
 *	 
 *          if (UEZTSClose(TS, aTSQueue) != UEZ_ERROR_NONE) {
 *              // error closing the device
 *          }
 *
 *  	} else {
 *          // an error occurred opening the device
 *  	}
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTSClose(T_uezDevice aDevice, T_uezQueue aTSQueue)
{
    T_uezError error;
    DEVICE_TOUCHSCREEN **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Remove queue first
    UEZTSRemoveQueue(aDevice, aTSQueue);

    return (*p)->Close((void *)p);
}

/*---------------------------------------------------------------------------*
 * Routine:  IUEZTSGrab
 *---------------------------------------------------------------------------*/
/**
 *  Grab exclusive control over the uEZ TS globals.
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTS.h>
 *
 *  IUEZTSGrab();
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void IUEZTSGrab(void)
{
    UEZSemaphoreGrab(G_tsSem, UEZ_TIMEOUT_INFINITE);
}

/*---------------------------------------------------------------------------*
 * Routine:  IUEZTSRelease
 *---------------------------------------------------------------------------*/
/**
 *  Release exclusive control over the uEZ TS globals.
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTS.h>
 *
 *  IUEZTSRelease();
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void IUEZTSRelease(void)
{
    UEZSemaphoreRelease(G_tsSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  IFindOrAddDevice
 *---------------------------------------------------------------------------*/
/**
 *  Search the list of devices for a matching device handle.  If they
 *      match, return the associated device entry in the device list.
 *      If not found, create a new device entry and return.
 *
 *  @param [in]    aDevice       	Device to find
 *
 *  @return        *T_tsDevice   	Device entry found or added.  Returns
 *                                      NULL if cannot add.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTS.h>
 *
 *  T_tsDevice *p_tsd;
 *  IUEZTSGrab();
 *  p_tsd = IFindOrAddDevice(aTSDevice);
 *  IUEZTSRelease();
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_tsDevice *IFindOrAddDevice(T_uezDevice aDevice)
{
    T_tsDevice *p;

    // Search the linked list of devices
    p = G_tsDevices;
    while (p) {
        if (p->iDevice == aDevice)
            break;
        p = p->iNext;
    }

    // Did we find it?
    if (!p) {
        // Didn't find it, add it to front of list
        p = UEZMemAlloc(sizeof(T_tsDevice));
        if (!p)
            return p;
        p->iNext = G_tsDevices;
        p->iDevice = aDevice;
        p->iQueueList = 0;
        G_tsDevices = p;
    }

    return p;
}


/*---------------------------------------------------------------------------*
 * Routine:  UEZTSGetReading
 *---------------------------------------------------------------------------*/
/**
 *  Forces the touch screen to poll for an immediate reading.
 *
 *  @param [in]    aDevice     		Touchscreen device to read
 *
 *  @param [in]    *aReading   	 	Pointer to reading structure to use
 *
 *  @return        T_uezError   	Error code, or UEZ_ERROR_NONE if read.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTS.h>
 *
 *  T_uezDevice TS;
 *  T_uezQueue aTSQueue;
 *  T_uezTSReading aReading;
 *  // Setup a queue to receive touch screen events
 *  if (UEZQueueCreate(1, sizeof(T_uezTSReading), &aTSQueue) != UEZ_ERROR_NONE) {
 *      EPRINTF("Could not allocate queue for touchscreen\n");
 *  } else {
 *  	if (UEZTSOpen("Touchscreen", &TS, &aTSQueue) == UEZ_ERROR_NONE) {
 *
 *          UEZTSGetReading(TS, &aReading);
 *
 *      } else {
 *          // an error occurred opening the device
 *      }
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTSGetReading(T_uezDevice aDevice, T_uezTSReading *aReading)
{
    T_uezError error;
    DEVICE_TOUCHSCREEN **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    error = (*p)->Poll((void *)p, aReading);
    if (error)
        return error;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZTSCalibrationStart
 *---------------------------------------------------------------------------*/
/**
 *  Put the touchscreen in calibration mode.  Readings will be taken
 *      and added to the calibration system.
 *
 *  @param [in]    aTSDevice      Touchscreen device to calibrate
 *
 *  @return        T_uezError 	Error code, or UEZ_ERROR_NONE if read.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTS.h>
 *
 *  T_uezDevice TS;
 *  UEZTSCalibrationStart(TS);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTSCalibrationStart(T_uezDevice aTSDevice)
{
    T_uezError error;
    DEVICE_TOUCHSCREEN **p;

    error = UEZDeviceTableGetWorkspace(aTSDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    error = (*p)->CalibrationStart((void *)p);
    if (error)
        return error;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZTSCalibrationEnd
 *---------------------------------------------------------------------------*/
/**
 *  Finishes the calibration and calculates the final setting.
 *
 *  @param [in]    aTSDevice   		Touchscreen device
 *
 *  @return        T_uezError    	Error code, or UEZ_ERROR_NONE if read.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTS.h>
 *
 *  T_uezDevice TS;
 *  UEZTSCalibrationStart(TS);
 *  // take readings here - refer to CalibrateProcedure() in Calibration.c
 *  UEZTSCalibrationEnd(TS);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTSCalibrationEnd(T_uezDevice aTSDevice)
{
    T_uezError error;
    DEVICE_TOUCHSCREEN **p;

    error = UEZDeviceTableGetWorkspace(aTSDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    error = (*p)->CalibrationEnd((void *)p);
    if (error)
        return error;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZTSCalibrationAddReading
 *---------------------------------------------------------------------------*/
/**
 *  Adds a data point to the touch screen calibration system.
 *
 *  @param [in]    aTSDevice   			Touchscreen device
 *
 *  @param [in]    *aReadingTaken 		Reading taken using UEZTSGetReading.
 *
 *  @param [in]    *aReadingExpected 	Reading that was expected (ideal).
 *
 *  @return        T_uezError          	Error code, or UEZ_ERROR_NONE if read.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTS.h>
 *
 *  T_uezDevice TS;
 *  T_uezTSReading aReadingTaken;
 *  T_uezTSReading aReadingExpected[5];
 *  UEZTSCalibrationAddReading(TS, &aReadingTaken, &aReadingExpected[1]);
 *
 *  UEZTSCalibrationStart(TS);
 *  // take readings here - refer to CalibrateProcedure() in Calibration.c
 *  UEZTSCalibrationEnd(TS);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTSCalibrationAddReading(
            T_uezDevice aTSDevice,
            const T_uezTSReading *aReadingTaken,
            const T_uezTSReading *aReadingExpected)
{
    T_uezError error;
    DEVICE_TOUCHSCREEN **p;

    error = UEZDeviceTableGetWorkspace(aTSDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    error = (*p)->CalibrationAddReading((void *)p, aReadingTaken, aReadingExpected);
    if (error)
        return error;

    return UEZ_ERROR_NONE;
}


/*---------------------------------------------------------------------------*
 * Routine:  AllocateTsQueue
 *---------------------------------------------------------------------------*/
/**
 *  Return a TS queue.
 *
 *  @return        T_tsQueue*	TS Queue pointer
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTS.h>
 *
 *  T_tsQueue *p_q;
 *  p_q = AllocateTsQueue(); // see UEZTSAddQueue
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_tsQueue *AllocateTsQueue( void )
{
T_tsQueue *tsq;
#ifdef NO_DYNAMIC_MEMORY_ALLOC

	tsq = xFreeTsList;
	xFreeTsList = xFreeTsList->iNext;
#else
  	tsq =  UEZMemAlloc(sizeof(T_tsQueue));
#endif
	return tsq;
}

/*---------------------------------------------------------------------------*
 * Routine:  ReleaseTsQueue
 *---------------------------------------------------------------------------*/
/**
 *  Return a TS queue to the free list.
 *
 *  @param [in]    *tsq 	TS Queue pointer
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTS.h>
 *
 *  T_tsQueue *p;
 *  ReleaseTsQueue(p); // see UEZTSRemoveQueue
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void ReleaseTsQueue( T_tsQueue *tsq )
{
#ifdef NO_DYNAMIC_MEMORY_ALLOC
  	tsq->iNext = xFreeTsList;
	xFreeTsList = tsq;
#else
	UEZMemFree(tsq);
#endif  
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZTSAddQueue
 *---------------------------------------------------------------------------*/
/**
 *  Adds a data point to the touch screen calibration system.
 *
 *  @param [in]    aTSDevice        	Touchscreen device
 *
 *  @param [in]    aEventQueue     	Queue to receive touchscreen events
 *
 *  @return        T_uezError      	If the queue added, returns
 *                                  UEZ_ERROR_NONE. If the queue cannot be 
 *                                  added, returns UEZ_ERROR_OUT_OF_MEMORY.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTS.h>
 *
 *  T_uezDevice aTSDevice;
 *  T_uezQueue aEventQueue;
 *  UEZTSAddQueue(aDevice, aEventQueue); // see UEZTSOpen
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTSAddQueue(T_uezDevice aTSDevice, T_uezQueue aEventQueue)
{
    T_tsDevice *p_tsd;
    T_tsQueue *p_q;

    IUEZTSGrab();
    p_tsd = IFindOrAddDevice(aTSDevice);
    if (!p_tsd) {
        IUEZTSRelease();
        return UEZ_ERROR_OUT_OF_MEMORY;
    }
    p_q = AllocateTsQueue();
    if (!p_q)  {
        IUEZTSRelease();
        return UEZ_ERROR_OUT_OF_MEMORY;
    }
    p_q->iQueue = aEventQueue;
    p_q->iNext = p_tsd->iQueueList;

    p_tsd->iQueueList = p_q;

    IUEZTSRelease();

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZTSRemoveQueue
 *---------------------------------------------------------------------------*/
/**
 *  Remove queue receiving touch screen events for associate device.
 *
 *  @param [in]    aTSDevice      	Touchscreen device
 *
 *  @param [in]    aEventQueue     	Queue receiving touchscreen events
 *
 *  @return        T_uezError      	If the queue added, returns
 *                                  UEZ_ERROR_NONE. If the queue cannot be 
 *                                  added, returns UEZ_ERROR_OUT_OF_MEMORY.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTS.h>
 *
 *  T_uezDevice aTSDevice;
 *  T_uezQueue aEventQueue;
 *  UEZTSRemoveQueue(aDevice, aEventQueue); // see UEZTSClose
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTSRemoveQueue(T_uezDevice aTSDevice, T_uezQueue aEventQueue)
{
    T_tsDevice *p_tsd;
    T_tsQueue *p_prev = 0;
    T_tsQueue *p;

    IUEZTSGrab();
    p_tsd = IFindOrAddDevice(aTSDevice);
    p = p_tsd->iQueueList;
    while (p)  {
        // Delete
        if (p->iQueue == aEventQueue)
            break;
        p_prev = p;
        p = p->iNext;
    }
    if (p)  {
        // Remove from the list of queues
        if (p_prev == 0)
            p_tsd->iQueueList = p->iNext;
        else
            p_prev->iNext = p->iNext;

        // Free the memory
		ReleaseTsQueue(p);
    }
    IUEZTSRelease();

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZTSSetTouchDetectSensitivity
 *---------------------------------------------------------------------------*/
/**
 *  Change the low (press) and high (release) points of the touch
 *      detection code.  These points are hardware specific, but the values
 *      passed are 0-0xFFFF as a scale between 0 V and Ref Voltage.  Notice
 *      that putting a gap between high and low provides a range where
 *      the press/release is reported as the previous press/release until
 *      the threshold is met.
 *
 *  @param [in]    aTSDevice      Touchscreen device
 *
 *  @param [in]    aLowLevel	Low (press) reference level (0-0xFFFF)
 *
 *  @param [in]    aHighLevel   High (release) reference level (0-0xFFFF)
 *    
 *  @return        T_uezError  	If the queue added, returns UEZ_ERROR_NONE. If
 *                              the queue cannot be added, returns
 *                              UEZ_ERROR_OUT_OF_MEMORY.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTS.h>
 *
 *  T_uezDevice TS;
 *  UEZTSSetTouchDetectSensitivity(TS, 0x2000, 0x6000);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTSSetTouchDetectSensitivity(
                T_uezDevice aTSDevice,
                TUInt16 aLowLevel,
                TUInt16 aHighLevel)
{
    T_uezError error;
    DEVICE_TOUCHSCREEN **p;

    error = UEZDeviceTableGetWorkspace(aTSDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SetTouchDetectSensitivity((void *)p, aLowLevel, aHighLevel);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZTS.c
 *-------------------------------------------------------------------------*/
