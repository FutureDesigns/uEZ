/*-------------------------------------------------------------------------*
 * File:  MQTT_Callbacks.c
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
#include "MQTT_Callbacks.h"

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
//static T_MQTTSettings G_MQTTSettings = { "00.0 C", "-0.0X -0.0Y -0.0Z"};

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

void MQTT_GetSettings(T_MQTTSettings *aSettings)
{
#if (COMPILER_TYPE != VisualC)
#if 0
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
            G_Temperature = reading/65536.0; // For uC/Probe Demo
            f = ((((TUInt32)reading) & 0xFFFF) * 10) >> 16;  // Convert to 1 digit decimal
            sprintf(G_MQTTSettings.iBoardTemp, "%02d.%01d C",
                                                i,
                                                f);
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
            
            G_XAccel = x; // For uC/Probe Demo
            G_YAccel = y; // For uC/Probe Demo
            G_ZAccel = z; // For uC/Probe Demo
            
            xi = (TInt32)(x * 0.6);
            xf = (((TInt32)(x * 10)) & 0xF)*6/10; // Convert to 1 digit decimal
            yi = (TInt32)(y * 0.6);
            yf = (((TInt32)(y * 10)) & 0xF)*6/10; // Convert to 1 digit decimal
            zi = (TInt32)(z * 0.6);
            zf = (((TInt32)(z * 10)) & 0xF)*6/10; // Convert to 1 digit decimal
            sprintf(G_MQTTSettings.iAccelerometer, "%+1d.%1dX %+1d.%1dY %+1d.%1dZ",
                                                xi,
                                                xf,
                                                yi,
                                                yf,
                                                zi,
                                                zf);
                                                #endif
        

#endif
    IGrab();
    //memcpy((void*)aSettings, (void*)&G_MQTTSettings, sizeof(T_SensorSettings));
    IRelease();
}

void MQTT_SetSettings(T_MQTTSettings *aSettings)
{
    IGrab();
    //sprintf(G_MQTTSettings.iAccelerometer, "%s", aSettings->iAccelerometer);
    IRelease();
}

void MQTT_Initialize()
{
    static TBool haveRun = EFalse;

    if(!haveRun)
        UEZSemaphoreCreateBinary(&G_Sem);
}
/*-------------------------------------------------------------------------*
 * End of File:  MQTT_Callbacks.c
 *-------------------------------------------------------------------------*/
