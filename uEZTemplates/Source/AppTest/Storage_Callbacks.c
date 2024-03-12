/*-------------------------------------------------------------------------*
 * File:  Storage_Callbacks.c
 *-------------------------------------------------------------------------*
 * Description:
 *     These callbacks constitute a flexible interface between application
 *     data and the application GUI.
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <Device/MassStorage.h>
#include <Global.h>
#include <Storage_Callbacks.h>
#include <stdio.h>
#include <uEZFile.h>

#include "Audio.h"
#include <Config_App.h>
#include <Source/Library/Audio/DAC/uEZDACWAVFile.h>
#include <stdio.h>
#include <string.h>
#include <uEZAudioMixer.h>
#include <uEZMemory.h>
#include <uEZStream.h>
#include "NVSettings.h"
#include "uEZPlatform.h"
#include "emWin/WindowManager.h"

#include "Audio.h"
#include <uEZToneGenerator.h>
#include <uEZAudioMixer.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define STORAGE_DIVIDER     1024

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static T_uezSemaphore G_Sem;
UEZ_ALIGN_VAR(4,char buffW[2048]);
UEZ_ALIGN_VAR(4,char buffR[2048] = {0});
static TUInt16 G_StressTestLevel = 0;
static TBool G_TestRunning = EFalse;
static WM_HWIN G_hMultiedit = 0;
void IPrintF(const char *msg, ...);
static TUInt16 testFileSize = 0;
static TUInt16 testIterations = 0;
static char G_StorageDevice = 1;
char buffer[128];

extern TBool G_USBFlash_inserted;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

static void IGrab(void)
{
    UEZSemaphoreGrab(G_Sem, UEZ_TIMEOUT_INFINITE);
}

static void IRelease(void)
{
    UEZSemaphoreRelease(G_Sem);
}


/*-------------------------------------------------------------------------*
 * Formats the selected storage medium
 *-------------------------------------------------------------------------*/
void Storage_FormatMedium(U8 device)
{
    T_uezDevice tone;
    IGrab();

    if(UEZToneGeneratorOpen("Speaker", &tone) == UEZ_ERROR_NONE) {
       UEZAudioMixerUnmute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
       UEZToneGeneratorPlayTone(tone, TONE_GENERATOR_HZ(1500), 25);

       UEZToneGeneratorPlayTone(tone, TONE_GENERATOR_HZ(1000), 25);
       UEZAudioMixerMute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
       UEZToneGeneratorClose(tone);
    }

    switch(device)
    {
    case 0:
      uEZFormatDrive('0'); // flash drive
      break;
    case 1:
      uEZFormatDrive('1'); // sd card
      break;
    default:
      IPrintF("Format failed: No valid medium selected!");
    }
    IRelease();
}

/*-------------------------------------------------------------------------*
 * Performs file test on selected storage medium
 *-------------------------------------------------------------------------*/
void Storage_FileTest(U8 device, U8 size, U8 duration)
{
    IGrab();

    if(device == 0) {
      G_StorageDevice = '0';
    }
    if(device == 1) {
      G_StorageDevice = '1';
    }    

    if (STORAGE_FILE_TEST_SMALL == size) {
      testFileSize = 255;//255;
      if (STORAGE_FILE_LEN_SHORT == duration) {
        testIterations = 300;
      } else if (STORAGE_FILE_LEN_NORMAL == duration) {
        testIterations = 600;
      } else if (STORAGE_FILE_LEN_LONG == duration) {
        testIterations = 1024;
      } else {
        testIterations = 300;
      }
    } else if (STORAGE_FILE_TEST_MEDIUM == size) {
      testFileSize = 350;////511;
      if (STORAGE_FILE_LEN_SHORT == duration) {
        testIterations = 150;
      } else if (STORAGE_FILE_LEN_NORMAL == duration) {
        testIterations = 300;
      } else if (STORAGE_FILE_LEN_LONG == duration) {
        testIterations = 1024;
      } else {
        testIterations = 150;
      }
    } else if (STORAGE_FILE_TEST_LARGE == size) {
      testFileSize = 2047;
      if (STORAGE_FILE_LEN_SHORT == duration) {
        testIterations = 50;
      } else if (STORAGE_FILE_LEN_NORMAL == duration) {
        testIterations = 200;
      } else if (STORAGE_FILE_LEN_LONG == duration) {
        testIterations = 500;
      } else {
        testIterations = 50;
      }
    } else {
      testFileSize = 255;
      if (STORAGE_FILE_LEN_SHORT == duration) {
        testIterations = 150;
      } else if (STORAGE_FILE_LEN_NORMAL == duration) {
        testIterations = 300;
      } else if (STORAGE_FILE_LEN_LONG == duration) {
        testIterations = 1024;
      } else {
        testIterations = 150;
      }
    }
    
    if(G_TestRunning == ETrue) {
      G_TestRunning = EFalse;    
    } else {
      G_TestRunning = ETrue;
    }

    IRelease();
    return;
}

 /*---------------------------------------------------------------------------*
 * Task:  StressTask
 *---------------------------------------------------------------------------*
 * Description:
 *      Task that deliberately stresses the CPU for troubleshooting purposes.
 * Inputs:
 *      T_uezTask aMyTask            -- Handle to this task
 *      void *aParams               -- Parameters.  Not used.
 * Outputs:
 *      TUInt32                     -- Never returns.
 *---------------------------------------------------------------------------*/
TUInt32 StressTask(T_uezTask aMyTask, void *aParams)
{
   PARAM_NOT_USED(aMyTask);
   PARAM_NOT_USED(aParams);
   while(1) {
      if(G_StressTestLevel > 0) {
        RTOS_ENTER_CRITICAL();
        UEZBSPDelayMS(G_StressTestLevel);
        RTOS_EXIT_CRITICAL();
        UEZBSPDelayMS(G_StressTestLevel);
      }
      UEZTaskDelay(1500-G_StressTestLevel-G_StressTestLevel);
    }
}

TUInt32 SDTask(T_uezTask aMyTask, void *aParams)
{
  PARAM_NOT_USED(aMyTask);
  PARAM_NOT_USED(aParams);
  T_uezFile file;
  T_uezDevice tone;
  char fileName[14] = {0};  // first char is drive number path
  char syncPath[3] = {'1',':',0}; // first char is drive number path
  TUInt32 numWritten;
  TUInt32 numRead;
  int cmpRes;
  while(1) {
    UEZTaskDelay(500);
    if(G_TestRunning == ETrue) {

      syncPath[0] = G_StorageDevice; // set the drive paths here
      fileName[0] = G_StorageDevice;
      for (U16 i = 0; i < testFileSize; i++) {
        buffW[i] = (U16) i;
      }

      if(UEZToneGeneratorOpen("Speaker", &tone) == UEZ_ERROR_NONE) {
         UEZAudioMixerUnmute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
         UEZToneGeneratorPlayTone(tone, TONE_GENERATOR_HZ(1500), 25);

         UEZToneGeneratorPlayTone(tone, TONE_GENERATOR_HZ(1000), 25);
         UEZAudioMixerMute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
         UEZToneGeneratorClose(tone);
      }

      for (int r = 0; r < 50; r++) {
        IPrintF("Begin file test (%d bytes x%d)\n\n", testFileSize, testIterations);
        for (U16 i = 0; i < testIterations; i++) {
            sprintf(&fileName[1], ":TA%d.TXT", i);
            //fileName[0] = G_StorageDevice;
            if(G_TestRunning == ETrue) {
              if (UEZFileOpen(fileName, FILE_FLAG_WRITE, &file) == UEZ_ERROR_NONE) {
                  if (UEZFileWrite(file, buffW, testFileSize, &numWritten) != UEZ_ERROR_NONE) {
                      IPrintF("File Write Error");
                  }
                  if (UEZFileSystemSync(syncPath) != UEZ_ERROR_NONE) {
                      IPrintF("Sync Error");
                  }
                  if (UEZFileClose(file) != UEZ_ERROR_NONE) {
                      IPrintF("File Close Error");
                  }
                  IPrintF( "Wrote: %s...\n", fileName);
              } else {
                  IPrintF("File Open Error: %s\n", fileName);
              }
            }
            UEZTaskDelay(10); // delay to allow user interface to still be responsive to allow user to stop test
        }

        for (U16 i = 0; i < testIterations; i++) {
            sprintf(&fileName[1], ":TA%d.TXT", i);
            //fileName[0] = G_StorageDevice;
            if(G_TestRunning == ETrue) {
              if (UEZFileOpen(fileName, FILE_FLAG_READ_ONLY, &file) == UEZ_ERROR_NONE) {
                  if (UEZFileRead(file, buffR, testFileSize, &numRead) != UEZ_ERROR_NONE) {
                      IPrintF("File Read Error: %s\n", fileName);
                  } else {
                      cmpRes = memcmp(buffW, buffR, testFileSize);
                      if (cmpRes != 0) {
                          IPrintF("Compare Failed: %i\n", cmpRes);
                      } else {
                          IPrintF("File OK: %s\n", fileName);
                      }
                  }
                  if (UEZFileClose(file) != UEZ_ERROR_NONE) {
                      IPrintF("File Close Error: %s\n", fileName);
                  }
                  if (UEZFileSystemSync(syncPath) != UEZ_ERROR_NONE) {
                      IPrintF("Sync Error");
                  }
              } else {
                  IPrintF("File Open Error: %s\n", fileName);
              }
            }
            UEZTaskDelay(10); // delay to allow user interface to still be responsive to allow user to stop test
        }
            
        IPrintF("\nEnd file test\n");
        IPrintF("Stress Level: %u\n", G_StressTestLevel);
        IPrintF("Run: %u\n\n", r);

        
        if(UEZToneGeneratorOpen("Speaker", &tone) == UEZ_ERROR_NONE) {
           UEZAudioMixerUnmute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
           UEZToneGeneratorPlayTone(tone, TONE_GENERATOR_HZ(1500), 25);

           UEZToneGeneratorPlayTone(tone, TONE_GENERATOR_HZ(1000), 25);
           UEZAudioMixerMute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
           UEZToneGeneratorClose(tone);
        }

        if(G_TestRunning == ETrue) {
           G_StressTestLevel +=30;
        } else {
           break;
        }
      }
      G_TestRunning = EFalse;
    } else { // not running test, can poll USB status
        if (G_USBFlash_inserted == EFalse) {
#if (UEZ_ENABLE_USB_HOST_STACK == 1)
          G_USBFlash_inserted = Storage_PrintInfo('0');
#endif
          if (G_USBFlash_inserted == ETrue) { // force repaint
            WindowManager_InvalidateCurrentWindow();
          }
        }
    }
  }
}


/*-------------------------------------------------------------------------*
 * Starts the CPU stress task for storage reliability testing
 *-------------------------------------------------------------------------*/
void Storage_BeginStressTest(void)
{  
  if(G_StressTestLevel < 0xFFFF) {
    G_StressTestLevel +=250;
  }  
  IPrintF("Stress Level Up: %u\n", G_StressTestLevel);  
}

/*-------------------------------------------------------------------------*
 * Ends the CPU stress task
 *-------------------------------------------------------------------------*/
void Storage_EndStressTest(void)
{
  if(G_StressTestLevel > 250) {
    G_StressTestLevel -=250;
  }
  IPrintF("Stress Level Dn: %u\n", G_StressTestLevel);
}

/*-------------------------------------------------------------------------*
 * Records an image of the current GUI state
 *-------------------------------------------------------------------------*/
void Storage_CaptureScreen(U8 device)
{
  if(device == 0) {
    WindowManager_SaveScreenShotBMP('0');
  }
  if(device == 1) {
    WindowManager_SaveScreenShotBMP('1');
  }  
}

/*---------------------------------------------------------------------------*
 * Routine:    IPrintF
 *---------------------------------------------------------------------------*/
/** Prints a formatted message simultaneously to the multiedit
 *  widget and serial port output.
 *     
 *  @param [in] msg         The format string to print, followed by any number
 *                          of formatting arguments as in printf().
 *  @return                 void
 */
 /*---------------------------------------------------------------------------*/
void  IPrintF(const char *msg, ...)
{
  va_list args;
  va_start(args, msg);
  vsprintf(buffer, msg, args);
  printf(buffer);
  if(G_hMultiedit != 0){
    if((uint32_t) MULTIEDIT_GetTextSize(G_hMultiedit) > (STORAGE_ME_BUFF_SIZE-sizeof(buffer))) {
      MULTIEDIT_SetText(G_hMultiedit, buffer);

    } else {
      MULTIEDIT_AddText(G_hMultiedit, buffer);
    }
  }
  va_end(args);
}

/*---------------------------------------------------------------------------*
 * Routine:  uEZFormatDrive
 *---------------------------------------------------------------------------*/
/**
 * Format the chosen drive with either FAT or EXFAT file system
 *
 * @param[in]   driveLetter         Drive number '0', '1', etc as character.
 *
 * @return T_uezError               Error code, if any. UEZ_ERROR_OK if
 *                                  successful.

 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <NVSettings.h>
 *
 *  if (uEZFormatDrive('1') == UEZ_ERROR_NONE) {
 *      // the format was successful
 *  } else {
 *      // an error occurred 
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError uEZFormatDrive(char driveLetter) {
  T_uezFile file;
  char fileName[14] = {0};
  char drivePath[4] = {driveLetter,':','/',0}; // logical drive path
  uint64_t totalBytes = 0;
  float sizeDecimal = 0.0;
  T_uezDevice tone;

  if(UEZToneGeneratorOpen("Speaker", &tone) == UEZ_ERROR_NONE) {
     UEZAudioMixerUnmute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
     UEZToneGeneratorPlayTone(tone, TONE_GENERATOR_HZ(1500), 25);
  }

  IPrintF("Formatting Drive....");
  if (UEZFileMKFS(driveLetter) != UEZ_ERROR_NONE) {
     IPrintF("FAIL: Format Error\n");
     return UEZ_ERROR_FAIL;
  } else {
     IPrintF("Drive Formatted Successfully.\n");
  }

  sprintf(fileName,"%c:REPORT.TXT", driveLetter);
  if (UEZFileOpen(fileName, FILE_FLAG_WRITE, &file) == UEZ_ERROR_NONE) {
    TUInt32 numWritten;
    char buff[80] = {0};

    sprintf(buff, "Device Formated:\n");
    UEZFileWrite(file, buff, strlen(buff), &numWritten);
    IPrintF(buff);

    T_msSizeInfo aDeviceInfo;
    if (UEZFileSystemGetStorageInfo(drivePath, &aDeviceInfo) == UEZ_ERROR_NONE) {
        totalBytes = ((uint64_t) aDeviceInfo.iNumSectors) * aDeviceInfo.iSectorSize;
        sizeDecimal = totalBytes/STORAGE_DIVIDER/STORAGE_DIVIDER; // MB
        sprintf(buff, "Storage Medium Report:\n  Sectors: %u\n", aDeviceInfo.iNumSectors);           
        UEZFileWrite(file, buff, strlen(buff), &numWritten);
        IPrintF(buff);

        sprintf(buff, "  Sector Size: %u\n  Block Size: %u\n",
            aDeviceInfo.iSectorSize, aDeviceInfo.iBlockSize);        
        UEZFileWrite(file, buff, strlen(buff), &numWritten);
        IPrintF(buff);
    
        if(sizeDecimal > STORAGE_DIVIDER) {
          sizeDecimal = sizeDecimal/STORAGE_DIVIDER; // GB 
          if(sizeDecimal > STORAGE_DIVIDER) {
            sizeDecimal = sizeDecimal/STORAGE_DIVIDER; // TB 
            if(sizeDecimal > STORAGE_DIVIDER) {
              sizeDecimal = sizeDecimal/STORAGE_DIVIDER; // PB
              sprintf(buff, "  Total Size: %f PB\n", (double) sizeDecimal); // In a real project you probably want to force printf into normal float.
            } else {          
              sprintf(buff, "  Total Size: %f TB\n", (double) sizeDecimal);
            }
          } else {          
            sprintf(buff, "  Total Size: %f GB\n",  (double) sizeDecimal);
          }
        } else {
          sprintf(buff, "  Total Size: %f MB\n", (double) sizeDecimal);
        }
        UEZFileWrite(file, buff, strlen(buff), &numWritten);
        IPrintF(buff);
    }

    T_uezFileSystemVolumeInfo aFSInfo;
    if (UEZFileSystemGetVolumeInfo(drivePath, &aFSInfo) == UEZ_ERROR_NONE) {
        sprintf(buff, "File System Report:\n");
        UEZFileWrite(file, buff, strlen(buff), &numWritten);
        IPrintF(buff);

        sprintf(buff, "  Bytes Per Sector: %u\n  Sectors Per Cluster: %u\n", 
            aFSInfo.iBytesPerSector, aFSInfo.iSectorsPerCluster);
        UEZFileWrite(file, buff, strlen(buff), &numWritten);
        IPrintF(buff);

        sprintf(buff, "  Num Clusters Total: %u\n  Num Clusters Free: %u\n",
            aFSInfo.iNumClustersTotal, aFSInfo.iNumClustersFree);
        UEZFileWrite(file, buff, strlen(buff), &numWritten);
        IPrintF(buff);
    }

    if(UEZToneGeneratorOpen("Speaker", &tone) == UEZ_ERROR_NONE) {
       UEZToneGeneratorPlayTone(tone, TONE_GENERATOR_HZ(1000), 25);
       UEZAudioMixerMute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
       UEZToneGeneratorClose(tone);
    }

    if (UEZFileClose(file) != UEZ_ERROR_NONE) {             
      return UEZ_ERROR_FAIL;// Error closing file
    }
  } else {
     return UEZ_ERROR_FAIL; // Error opening file
  }
  return UEZ_ERROR_NONE;
}

void Storage_SetLogHandel(WM_HWIN hMultiedit)
{
    G_hMultiedit = hMultiedit;
}

//Initialize
void Storage_Initialize(void)
{
    static TBool haveRun = EFalse;

    if(!haveRun) {
        UEZTaskCreate(StressTask, "Stress", 256, (void *)0, UEZ_PRIORITY_NORMAL, 0);
        UEZTaskCreate(SDTask, "SDTest", 10240, (void *)0, UEZ_PRIORITY_HIGH, 0);
        UEZSemaphoreCreateBinary(&G_Sem);
    }
}

TBool Storage_PrintInfo(char driveLetter) {
    char drivePath[4] = {driveLetter,':','/',0};
    uint64_t totalBytes = 0;
    float sizeDecimal = 0.0;

    T_msSizeInfo aDeviceInfo;
    if (UEZFileSystemGetStorageInfo(drivePath, &aDeviceInfo) == UEZ_ERROR_NONE) {
        printf("\nDrive %c information:\n", driveLetter);
        totalBytes = ((uint64_t) aDeviceInfo.iNumSectors) * aDeviceInfo.iSectorSize;
        sizeDecimal = totalBytes/STORAGE_DIVIDER/STORAGE_DIVIDER; // MB
        printf("Storage Medium Report:\n  Sectors: %u\n", aDeviceInfo.iNumSectors);
        printf("  Sector Size: %u\n  Block Size: %u\n",
            aDeviceInfo.iSectorSize, aDeviceInfo.iBlockSize);

        if(sizeDecimal > STORAGE_DIVIDER) {
          sizeDecimal = sizeDecimal/STORAGE_DIVIDER; // GB 
          if(sizeDecimal > STORAGE_DIVIDER) {
            sizeDecimal = sizeDecimal/STORAGE_DIVIDER; // TB 
            if(sizeDecimal > STORAGE_DIVIDER) {
              sizeDecimal = sizeDecimal/STORAGE_DIVIDER; // PB
              printf("  Total Size: %f PB\n", (double) sizeDecimal); // In a real project you probably want to force printf into normal float.
            } else {          
              printf("  Total Size: %f TB\n", (double) sizeDecimal);
            }
          } else {          
            printf("  Total Size: %f GB\n", (double) sizeDecimal);
          }
        } else {
          printf("  Total Size: %f MB\n", (double) sizeDecimal);
        }
    } else {
      return EFalse; // Didn't find the storage device.
    }

    T_uezFileSystemVolumeInfo aFSInfo;
    if (UEZFileSystemGetVolumeInfo(drivePath, &aFSInfo) == UEZ_ERROR_NONE) {
        printf("File System Report:\n");
        printf("  Bytes Per Sector: %u\n  Sectors Per Cluster: %u\n",
            aFSInfo.iBytesPerSector, aFSInfo.iSectorsPerCluster);
        printf("  Num Clusters Total: %u\n  Num Clusters Free: %u\n",
            aFSInfo.iNumClustersTotal, aFSInfo.iNumClustersFree);
    }
    
    return ETrue; // found the storage device (even if filesystem isn't valid)
}

/*-------------------------------------------------------------------------*
 * End of File:  Storage_Callbacks.c
 *-------------------------------------------------------------------------*/
