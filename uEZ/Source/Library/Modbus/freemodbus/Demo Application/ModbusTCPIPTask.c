/* ------------------------ System includes ------------------------------- */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* ------------------------ RTOS includes --------------------------------- */
#include <uEZ.h>

/* ------------------------ LWIP includes --------------------------------- */
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/memp.h"

/* ------------------------ FreeModbus includes --------------------------- */
#include "mb.h"

/* ------------------------ Project includes ------------------------------ */


/* ------------------------ Defines --------------------------------------- */

#define PROG                    "FreeModbus"
#define REG_INPUT_START         1000
#define REG_INPUT_NREGS         4
#define REG_HOLDING_START       2000
#define REG_HOLDING_NREGS       130
#define COILS_START             3000
#define COILS_NCOILS            100
#define DISCRETE_INPUTS_START   4000
#define DISCRETE_INPUTS_NINPUTS 200

/* ----------------------- Bitmap Macros ------------------------------------*/
#define BM_SET(array, bitIndex)     (array[(bitIndex)/8] |= (1 << ((bitIndex) & 7)))
#define BM_CLEAR(array, bitIndex)   (array[(bitIndex)/8] &= ~(1 << ((bitIndex) & 7)))
#define BM_TEST(array, bitIndex)    (array[(bitIndex)/8] & (1 << ((bitIndex) & 7)))
#define BM_TOGGLE(array, bitIndex)  (array[(bitIndex)/8] ^= (1 << ((bitIndex) & 7)))

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];
static USHORT   usRegHoldingStart = REG_HOLDING_START;
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];

static USHORT   usCoilsStart = COILS_START;
static UCHAR    ucCoilsBuf[((COILS_NCOILS)/8) + 1];

static USHORT   usDiscreteInputsStart = DISCRETE_INPUTS_START;
static UCHAR    ucDiscreteInputsBuf[((DISCRETE_INPUTS_NINPUTS)/8) + 1];

/* ------------------------ Static functions ------------------------------ */
static void     vMBServerTask( void *arg );

static BOOL     eMBTCPConnectionAcceptCB( T_uEZNetworkConnectionInfo *aConnection );

static void     eMBTCPConnectionCloseCB( T_uEZNetworkConnectionInfo *aConnection );

/* ------------------------ Implementation -------------------------------- */
void
ModbusTCPIPTask_Start(void)
{
    /* we can preset some callbacks and timer values before initializing the protocol stack */
    // xMBTCPPortSetConnectionTimeout(300);
    xMBTCPPortRegisterConnectionAttemptCB( eMBTCPConnectionAcceptCB);
    xMBTCPPortRegisterConnectionCloseCB( eMBTCPConnectionCloseCB);

    if( eMBTCPInit( MB_TCP_PORT_USE_DEFAULT ) != MB_ENOERR )
    {
        printf( "%s: can't initialize modbus stack!\r\n", PROG );
    }
    else if( eMBEnable(  ) != MB_ENOERR )
    {
        printf(  "%s: can't enable modbus stack!\r\n", PROG );
    }
}

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
    /* UCHAR           ucUnitAddress = xMBTCPPortGetUnitAddress(); */

    /* example code to retrieve connection info for the current request
        T_uEZNetworkConnectionInfo aConnection;

        if (xMBTCPPortGetConnectionInfo( &aConnection ))
        {
            printf("input register callback %d.%d.%d.%d\r\n", 
                    aConnection.iIPConnectedAddr[0],
                    aConnection.iIPConnectedAddr[1],
                    aConnection.iIPConnectedAddr[2],
                    aConnection.iIPConnectedAddr[3]);
        }
    */

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
    /* UCHAR           ucUnitAddress = xMBTCPPortGetUnitAddress(); */

    /* example code to retrieve connection info for the current request
        T_uEZNetworkConnectionInfo aConnection;

        if (xMBTCPPortGetConnectionInfo( &aConnection ))
        {
            printf("Holding register callback %d.%d.%d.%d\r\n", 
                    aConnection.iIPConnectedAddr[0],
                    aConnection.iIPConnectedAddr[1],
                    aConnection.iIPConnectedAddr[2],
                    aConnection.iIPConnectedAddr[3]);
        }
    */

    if( ( usAddress >= REG_HOLDING_START ) &&
        ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        switch ( eMode )
        {
            /* Pass current register values to the protocol stack. */
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( UCHAR ) ( usRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;

            /* Update current register values with new values from the
             * protocol stack. */
        case MB_REG_WRITE:
            while( usNRegs > 0 )
            {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
    USHORT          usIndex;       
    UCHAR           ucNBytes;
    /* UCHAR           ucUnitAddress = xMBTCPPortGetUnitAddress(); */

    /* example code to retrieve connection info for the current request
        T_uEZNetworkConnectionInfo aConnection;

        if (xMBTCPPortGetConnectionInfo( &aConnection ))
        {
            printf("coils callback %d.%d.%d.%d\r\n", 
                    aConnection.iIPConnectedAddr[0],
                    aConnection.iIPConnectedAddr[1],
                    aConnection.iIPConnectedAddr[2],
                    aConnection.iIPConnectedAddr[3]);
        }
    */

    if( ( usAddress >=COILS_START ) &&
        ( usAddress + usNCoils <=COILS_START + COILS_NCOILS ) )
    {
        iRegIndex = ( int )( usAddress - usCoilsStart );
        usIndex = 0;

        switch ( eMode )
        {
          /* Pass current coil values to the protocol stack. */
          case MB_REG_READ:
            // the last byte needs to be padded with 0s, so we will just 0 out everything
            // and set the discrete inputs that are true
            if( ( usNCoils & 0x0007 ) != 0 )
            {
                ucNBytes = ( UCHAR ) ( usNCoils / 8 + 1 );
            }
            else
            {
                ucNBytes = ( UCHAR ) ( usNCoils / 8 );
            }
            memset(pucRegBuffer, 0, ucNBytes);

            /* Pass current register values to the protocol stack. */
            while( usNCoils > 0 )
            {
                // if the input is true we need to set it true in the output
                if (BM_TEST(ucCoilsBuf, iRegIndex))
                {
                  BM_SET(pucRegBuffer, usIndex);
                }
        
                // increment/decrement our counters
                iRegIndex++;
                usIndex++;
                usNCoils--;
            }
            break;

            /* Update current coil values with new values from the
             * protocol stack. */
        case MB_REG_WRITE:
            while( usNCoils > 0 )
            {
                if (BM_TEST(pucRegBuffer, usIndex))
                {
                  BM_SET(ucCoilsBuf, iRegIndex);
                }
                else
                {
                  BM_CLEAR(ucCoilsBuf, iRegIndex);
                }

                iRegIndex++;
                usIndex++;
                usNCoils--;
            }
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
    USHORT          usOutIndex;       
    UCHAR           ucNBytes;
    /* UCHAR           ucUnitAddress = xMBTCPPortGetUnitAddress(); */

    /* example code to retrieve connection info for the current request
        T_uEZNetworkConnectionInfo aConnection;

        if (xMBTCPPortGetConnectionInfo( &aConnection ))
        {
            printf("discrete register callback %d.%d.%d.%d\r\n", 
                    aConnection.iIPConnectedAddr[0],
                    aConnection.iIPConnectedAddr[1],
                    aConnection.iIPConnectedAddr[2],
                    aConnection.iIPConnectedAddr[3]);
        }
    */

    if( ( usAddress >= DISCRETE_INPUTS_START ) &&
        ( usAddress + usNDiscrete <= DISCRETE_INPUTS_START + DISCRETE_INPUTS_NINPUTS ) )
    {
        iRegIndex = ( int )( usAddress - usDiscreteInputsStart );

        // the last byte needs to be padded with 0s, so we will just 0 out everything
        // and set the discrete inputs that are true
        if( ( usNDiscrete & 0x0007 ) != 0 )
        {
            ucNBytes = ( UCHAR ) ( usNDiscrete / 8 + 1 );
        }
        else
        {
            ucNBytes = ( UCHAR ) ( usNDiscrete / 8 );
        }
        memset(pucRegBuffer, 0, ucNBytes);

        usOutIndex = 0;

        /* Pass current register values to the protocol stack. */
        while( usNDiscrete > 0 )
        {
            // if the input is true we need to set it true in the output
            if (BM_TEST(ucDiscreteInputsBuf, iRegIndex))
            {
              BM_SET(pucRegBuffer, usOutIndex);
            }
          
            // increment/decrement our counters
            iRegIndex++;
            usOutIndex++;
            usNDiscrete--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBTCPSetSlaveID( UCHAR *ucSlaveID, BOOL *xIsRunning,
               UCHAR const *pucAdditional, USHORT *usAdditionalLen )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    UCHAR           ucUnitAddress = xMBTCPPortGetUnitAddress();
    UCHAR ucTestString[] = "TestString";

    if (ucUnitAddress == 1)
    {
        *ucSlaveID = 1;
        *xIsRunning = TRUE;
        *usAdditionalLen = strlen(ucTestString);
        memcpy( pucAdditional, ucTestString, ( size_t )*usAdditionalLen );
    }
    else
    {
        eStatus = MB_EINVAL;
    }
    return eStatus;
}

static BOOL
eMBTCPConnectionAcceptCB(T_uEZNetworkConnectionInfo *aConnection)
{
    /* returning FALSE will cause the connection to NOT be accepted */
    /* TRUE allows the connection to be accepted */
    printf("Connection accept callback %d.%d.%d.%d\r\n", 
    aConnection->iIPConnectedAddr[0],
    aConnection->iIPConnectedAddr[1],
    aConnection->iIPConnectedAddr[2],
    aConnection->iIPConnectedAddr[3]);
    
    return TRUE;
}

static void
eMBTCPConnectionCloseCB(T_uEZNetworkConnectionInfo *aConnection)
{
    /* purely informative callback, the connection will be closed */
    printf("Connection close callback %d.%d.%d.%d\r\n", 
    aConnection->iIPConnectedAddr[0],
    aConnection->iIPConnectedAddr[1],
    aConnection->iIPConnectedAddr[2],
    aConnection->iIPConnectedAddr[3]);
}