/*-------------------------------------------------------------------------*
 * File:  Storage_Callbacks.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef STORAGE_CALLBACK_H_
#define STORAGE_CALLBACK_H_
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include "GUI.h"
#include "BUTTON.h"
#include "FRAMEWIN.h"
#include "DIALOG.h"
#include "TEXT.h"
#include "WM.h"
#include <stdarg.h>
   
/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define STORAGE_FILE_TEST_SMALL     (0x00)
#define STORAGE_FILE_TEST_MEDIUM    (0x01)
#define STORAGE_FILE_TEST_LARGE     (0x02)

#define STORAGE_FILE_LEN_SHORT      (0x00)
#define STORAGE_FILE_LEN_NORMAL     (0x01)
#define STORAGE_FILE_LEN_LONG       (0x02)

#define STORAGE_ME_BUFF_SIZE        (512)

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void Storage_FormatMedium(U8 device);
void Storage_FileTest(U8 device, U8 size, U8 duration);
void Storage_CaptureScreen(U8 device);
void Storage_Initialize(void);
void Storage_SetLogHandel(WM_HWIN hMultiedit);
void Storage_BeginStressTest(void);
void Storage_EndStressTest(void);
void Storage_PrintInfo(char driveLetter);

#endif // STORAGE_CALLBACK_H_
/*-------------------------------------------------------------------------*
 * End of File:  Storage_Callbacks.h
 *-------------------------------------------------------------------------*/
