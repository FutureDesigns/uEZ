/*-------------------------------------------------------------------------*
 * File:  Sensor_Callbacks.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#if (COMPILER_TYPE != VisualC)
#include <uEZI2C.h>
#include <Device/I2CBus.h>
#include <uEZTemperature.h>
//#include <uEZAccelerometer.h>
#include <Device/Accelerometer.h>
#include <uEZDeviceTable.h>
#endif
#include "Sensor_Callbacks.h"
#include <Source/Library/SEGGER/SystemView/SEGGER_SYSVIEW.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static T_uezSemaphore G_Sem;
static T_SensorSettings G_SensorSettings = { "00.0 C", "-0.0X -0.0Y -0.0Z"};

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

float G_XAccel, G_YAccel, G_ZAccel; 	// Global Variables for tracing Demo
float G_Temperature;	                // Global Variable for tracing Demo


#define SEGGER_SYSVIEW_DATA_ID_ACCEL_X   (0)
#define SEGGER_SYSVIEW_DATA_ID_ACCEL_Y   (1)
#define SEGGER_SYSVIEW_DATA_ID_ACCEL_Z   (2)
 
#if (SEGGER_ENABLE_SYSTEM_VIEW == 1)
static const SEGGER_SYSVIEW_DATA_REGISTER regAccelDataX = {
    .ID = SEGGER_SYSVIEW_DATA_ID_ACCEL_X,
    .DataType = SEGGER_SYSVIEW_TYPE_FLOAT,
    .Offset = 0,
    .RangeMin = 0,
    .RangeMax = 0,
    .ScalingFactor = 1.0f, // must be > 0
    .sName = "Accel X",
    .sUnit = "g"
};
static const SEGGER_SYSVIEW_DATA_REGISTER regAccelDataY = {
    .ID = SEGGER_SYSVIEW_DATA_ID_ACCEL_Y,
    .DataType = SEGGER_SYSVIEW_TYPE_FLOAT,
    .Offset = 0,
    .RangeMin = 0,
    .RangeMax = 0,
    .ScalingFactor = 1.0f, // must be > 0
    .sName = "Accel Y",
    .sUnit = "g"
};
static const SEGGER_SYSVIEW_DATA_REGISTER regAccelDataZ = {
    .ID = SEGGER_SYSVIEW_DATA_ID_ACCEL_Z,
    .DataType = SEGGER_SYSVIEW_TYPE_FLOAT,
    .Offset = 0,
    .RangeMin = 0,
    .RangeMax = 0,
    .ScalingFactor = 1.0f, // must be > 0
    .sName = "Accel Z",
    .sUnit = "g"
};
#endif
const SEGGER_SYSVIEW_DATA_SAMPLE sampleAccelXdata = {
    .ID = SEGGER_SYSVIEW_DATA_ID_ACCEL_X,
    .pValue.pFloat= &G_XAccel,
};
const SEGGER_SYSVIEW_DATA_SAMPLE sampleAccelYdata = {
    .ID = SEGGER_SYSVIEW_DATA_ID_ACCEL_Y,
    .pValue.pFloat= &G_YAccel,
};
const SEGGER_SYSVIEW_DATA_SAMPLE sampleAccelZdata = {
    .ID = SEGGER_SYSVIEW_DATA_ID_ACCEL_Z,
    .pValue.pFloat= &G_ZAccel,
};

void Sensor_GetSettings(T_SensorSettings *aSettings)
{
#if (COMPILER_TYPE != VisualC)
    T_uezDevice temp;    
    TInt32 reading;
    TInt32 i, f, xi, xf, yi, yf, zi, zf; 
    double x, y, z;
    static DEVICE_Accelerometer **G_accel0;
    AccelerometerReading aReading;
    T_uezDevice acdevice;

    IGrab();
    if(UEZTemperatureOpen("Temp0", &temp) == UEZ_ERROR_NONE){
        if (UEZTemperatureRead(temp, &reading) == UEZ_ERROR_NONE){
            // This temperature sensor is 11 bits up to about 125 C.
            // 3 bits are fraction, 7 bits are integer, and 1 sign bit

            i = reading >> 16;
            G_Temperature = reading/65536.0; // For tracing demo
            f = ((((TUInt32)reading) & 0xFFFF) * 10) >> 16;  // Convert to 1 digit decimal
            sprintf(G_SensorSettings.iBoardTemp, "%02d.%01d C",
                                          (int) i,
                                          (int) f);
        }
        UEZTemperatureClose(temp);
    }    
    IRelease();
    
     UEZDeviceTableFind("Accel0", &acdevice);
     UEZDeviceTableGetWorkspace( acdevice,
                                (T_uezDeviceWorkspace **)&G_accel0);    
        x = y = z = 0;
        for (i = 0; i < 8; i++) {
            UEZTaskDelay(10);            
            (*G_accel0)->ReadXYZ(G_accel0, &aReading, 100);
                x += aReading.iX;
                y += aReading.iY;
                z += aReading.iZ;
            }            
            // Convert from signed 15.16 fixed format to doubles over 8 samples
            // The value is not in g's.  For example, the x, y, z is
            // (0.0, 0.0, -1.0) when at rest on a table.
            x /= 65536.0 * 0.8;
            y /= 65536.0 * 0.8;
            z /= 65536.0 * 0.8;
            
            G_XAccel = x; // For tracing demo
            G_YAccel = y; // For tracing demo
            G_ZAccel = z; // For tracing demo
#if (SEGGER_ENABLE_SYSTEM_VIEW == 1)
            SEGGER_SYSVIEW_SampleData(&sampleAccelXdata);
            SEGGER_SYSVIEW_SampleData(&sampleAccelYdata);
            SEGGER_SYSVIEW_SampleData(&sampleAccelZdata);
#endif
            
            xi = (TInt32)(x * 0.6);
            xf = (((TInt32)(x * 10)) & 0xF)*6/10; // Convert to 1 digit decimal
            yi = (TInt32)(y * 0.6);
            yf = (((TInt32)(y * 10)) & 0xF)*6/10; // Convert to 1 digit decimal
            zi = (TInt32)(z * 0.6);
            zf = (((TInt32)(z * 10)) & 0xF)*6/10; // Convert to 1 digit decimal
            sprintf(G_SensorSettings.iAccelerometer, "%+1d.%1dX %+1d.%1dY %+1d.%1dZ",
                                               (int) xi,
                                               (int) xf,
                                               (int) yi,
                                               (int) yf,
                                               (int) zi,
                                               (int) zf);
        
#else    
    sprintf(G_SensorSettings.iBoardTemp, "50.0 C");
    sprintf(G_SensorSettings.iAccelerometer , "50.0 C");    
#endif
    IGrab();
    memcpy((void*)aSettings, (void*)&G_SensorSettings, sizeof(T_SensorSettings));
    IRelease();
}

void Sensor_SetSettings(T_SensorSettings *aSettings)
{
    IGrab();
    sprintf(G_SensorSettings.iAccelerometer, "%s", aSettings->iAccelerometer);
    IRelease();
}

void Sensor_Initialize()
{
    static TBool haveRun = EFalse;

    if(!haveRun) {
        UEZSemaphoreCreateBinary(&G_Sem);
#if (SEGGER_ENABLE_SYSTEM_VIEW == 1)
        SEGGER_SYSVIEW_RegisterData((SEGGER_SYSVIEW_DATA_REGISTER*)&regAccelDataX);
        SEGGER_SYSVIEW_RegisterData((SEGGER_SYSVIEW_DATA_REGISTER*)&regAccelDataY);
        SEGGER_SYSVIEW_RegisterData((SEGGER_SYSVIEW_DATA_REGISTER*)&regAccelDataZ);
#endif
    }
}
/*-------------------------------------------------------------------------*
 * End of File:  Sensor_Callbacks.c
 *-------------------------------------------------------------------------*/
