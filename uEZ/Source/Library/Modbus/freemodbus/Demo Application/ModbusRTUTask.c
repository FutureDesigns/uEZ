/*-------------------------------------------------------------------------*
 * File:  ModbusRTUTask.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <string.h>
#include <uEZ.h>
#include "mb.h"
#include "mbport.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
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

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
static TUInt16   usRegInputStart = REG_INPUT_START;
static TUInt16   usRegInputBuf[REG_INPUT_NREGS];

static TUInt16   usRegHoldingStart = REG_HOLDING_START;
static TUInt16   usRegHoldingBuf[REG_HOLDING_NREGS];

static TUInt16   usCoilsStart = COILS_START;
static TUInt8    ucCoilsBuf[((COILS_NCOILS)/8) + 1];

static TUInt16   usDiscreteInputsStart = DISCRETE_INPUTS_START;
static TUInt8    ucDiscreteInputsBuf[((DISCRETE_INPUTS_NINPUTS)/8) + 1];

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
TUInt32 ModbusRTU_Task(T_uezTask aMyTask, void *aParams)
{
    const UCHAR     ucSlaveID[] = { 0xAA, 0xBB, 0xCC };
    eMBErrorCode    eStatus;

    eStatus = eMBInit( MB_RTU, 0x0A, 0, 115200, MB_PAR_NONE );

    eStatus = eMBSetSlaveID( 0x34, TRUE, ucSlaveID, 3 );

    /* Enable the Modbus Protocol Stack. */
    eStatus = eMBEnable(  );

    for( ;; )
    {
        ( void )eMBPoll(  );

        /* Here we simply count the number of poll cycles. */
        usRegInputBuf[0]++;
    }
}

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ =
                ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
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
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

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
            break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
    USHORT          usIndex;       
    UCHAR           ucNBytes;
    
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
            break;
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

void ModbusRTUTask_Start()
{
    static TBool haveRun = EFalse;

    if(!haveRun){
        UEZTaskCreate(ModbusRTU_Task, "RTU", (1 * 1024), (void *) 0, UEZ_PRIORITY_NORMAL, 0);
    }
}

/*-------------------------------------------------------------------------*
 * End of File:  ModbusRTUTask.c
 *-------------------------------------------------------------------------*/
