/******************************************************************************
* DISCLAIMER
* Please refer to http://www.renesas.com/disclaimer
******************************************************************************/
/***********************************************************************/
/*                                                                     */
/*  FILE        :ExMemory.h                                            */
/*  DATE        :2009/04/20                                            */
/*  DESCRIPTION :Interface to external memory manager                  */
/*                                                                     */
/*                                                                     */
/***********************************************************************/
#ifndef EXMEMORY_H
#define EXMEMORY_H

extern uI16 LCD_BusActive;       // flag indicating if we're running ExDMA on this line

/**********************************************************************************
Function Prototypes
***********************************************************************************/
void ExMemoryMonitorInit(void);
void EventManagerInit(void);
void FramesInit(void);
void ExMemoryAcquire(RLCD_TaskHandle handle);
void ExMemoryRelease(RLCD_TaskHandle handle);
uI08 * ExMemoryGet(uI16 size);
uI08 ExMemoryFree(uI08 *pPool);

#endif

