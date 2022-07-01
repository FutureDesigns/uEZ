/*-------------------------------------------------------------------------*
 * File:  LPC4357_UtilityFuncs.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Utilty functions that are specific to the LPC1768 processor.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <Config.h>
#include <uEZTypes.h>
#include <uEZ.h>
#include <uEZPlatform.h>
#include <uEZProcessor.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_UtilityFuncs.h>
#include <uEZBSP.h>
#include <uEZGPIO.h>
#include <uEZLCD.h>

/*---------------------------------------------------------------------------*
 * Routine:  ReadLE32U
 *---------------------------------------------------------------------------*
 * Description:
 *      Read Little Endian 32 bit unsigned value from memory
 * Inputs:
 *      volatile TUInt8 *pmem -- Pointer to memory to read from
 * Outputs:
 *      TUInt32 -- value read
 *---------------------------------------------------------------------------*/
TUInt32 ReadLE32U(volatile TUInt8 *pmem)
{
    TUInt32 val;

    ((TUInt8 *)&val)[0] = pmem[0];
    ((TUInt8 *)&val)[1] = pmem[1];
    ((TUInt8 *)&val)[2] = pmem[2];
    ((TUInt8 *)&val)[3] = pmem[3];

    return (val);
}

/*---------------------------------------------------------------------------*
 * Routine:  WriteLE32U
 *---------------------------------------------------------------------------*
 * Description:
 *      Write Little Endian 32 bit unsigned value into memory
 * Inputs:
 *      volatile TUInt8 *pmem -- Pointer to memory to write to
 *      TUInt32 val -- value to write
 *---------------------------------------------------------------------------*/
void WriteLE32U(volatile TUInt8 *pmem, TUInt32 val)
{
    pmem[0] = ((TUInt8 *)&val)[0];
    pmem[1] = ((TUInt8 *)&val)[1];
    pmem[2] = ((TUInt8 *)&val)[2];
    pmem[3] = ((TUInt8 *)&val)[3];
}

/*---------------------------------------------------------------------------*
 * Routine:  ReadBE32U
 *---------------------------------------------------------------------------*
 * Description:
 *      Read Big Endian 32 bit unsigned value from memory
 * Inputs:
 *      volatile TUInt8 *pmem -- Pointer to memory to read from
 * Outputs:
 *      TUInt32 -- value read
 *---------------------------------------------------------------------------*/
TUInt32 ReadBE32U(volatile TUInt8 *pmem)
{
    TUInt32 val;

    ((TUInt8 *)&val)[0] = pmem[3];
    ((TUInt8 *)&val)[1] = pmem[2];
    ((TUInt8 *)&val)[2] = pmem[1];
    ((TUInt8 *)&val)[3] = pmem[0];

    return (val);
}

/*---------------------------------------------------------------------------*
 * Routine:  WriteBE32U
 *---------------------------------------------------------------------------*
 * Description:
 *      Write Little Endian 32 bit unsigned value into memory
 * Inputs:
 *      volatile TUInt8 *pmem -- Pointer to memory to write to
 *      TUInt32 val -- value to write
 *---------------------------------------------------------------------------*/
void WriteBE32U(volatile TUInt8 *pmem, TUInt32 val)
{
    pmem[0] = ((TUInt8 *)&val)[3];
    pmem[1] = ((TUInt8 *)&val)[2];
    pmem[2] = ((TUInt8 *)&val)[1];
    pmem[3] = ((TUInt8 *)&val)[0];
}

/*---------------------------------------------------------------------------*
 * Routine:  ReadLE16U
 *---------------------------------------------------------------------------*
 * Description:
 *      Read Little Endian 16 bit unsigned value from memory
 * Inputs:
 *      volatile TUInt8 *pmem -- Pointer to memory to read from
 * Outputs:
 *      TUInt16 -- value read
 *---------------------------------------------------------------------------*/
TUInt16 ReadLE16U(volatile TUInt8 *pmem)
{
    TUInt16 val;

    ((TUInt8 *)&val)[0] = pmem[0];
    ((TUInt8 *)&val)[1] = pmem[1];

    return (val);
}

/*---------------------------------------------------------------------------*
 * Routine:  WriteLE16U
 *---------------------------------------------------------------------------*
 * Description:
 *      Write Little Endian 16 bit unsigned value into memory
 * Inputs:
 *      volatile TUInt8 *pmem -- Pointer to memory to write to
 *      TUInt16 val -- value to write
 *---------------------------------------------------------------------------*/
void WriteLE16U(volatile TUInt8 *pmem, TUInt16 val)
{
    pmem[0] = ((TUInt8 *)&val)[0];
    pmem[1] = ((TUInt8 *)&val)[1];
}

/*---------------------------------------------------------------------------*
 * Routine:  ReadBE16U
 *---------------------------------------------------------------------------*
 * Description:
 *      Read Big Endian 16 bit unsigned value from memory
 * Inputs:
 *      volatile TUInt8 *pmem -- Pointer to memory to read from
 * Outputs:
 *      TUInt16 -- value read
 *---------------------------------------------------------------------------*/
TUInt16 ReadBE16U(volatile TUInt8 *pmem)
{
    TUInt16 val;

    ((TUInt8 *)&val)[0] = pmem[1];
    ((TUInt8 *)&val)[1] = pmem[0];

    return (val);
}

/*---------------------------------------------------------------------------*
 * Routine:  WriteBE16U
 *---------------------------------------------------------------------------*
 * Description:
 *      Write Big Endian 16 bit unsigned value into memory
 * Inputs:
 *      volatile TUInt8 *pmem -- Pointer to memory to write to
 *      TUInt16 val -- value to write
 *---------------------------------------------------------------------------*/
void WriteBE16U(volatile TUInt8 *pmem, TUInt16 val)
{
    pmem[0] = ((TUInt8 *)&val)[1];
    pmem[1] = ((TUInt8 *)&val)[0];
}

/*-------------------------------------------------------------------------*
 * Function:  CPUDisableInterrupts
 *-------------------------------------------------------------------------*
 * Description:
 *      Disables all standard CPU interrupts (IRQ).
 *-------------------------------------------------------------------------*/
void CPUDisableInterrupts(void)
{
    __set_BASEPRI(16UL << (8 - 5));
}

/*-------------------------------------------------------------------------*
 * Function:  CPUEnableInterrupts
 *-------------------------------------------------------------------------*
 * Description:
 *      Enables all standard CPU interrupts (IRQ).
 *-------------------------------------------------------------------------*/
void CPUEnableInterrupts(void)
{
    __set_BASEPRI(0UL);
}

extern unsigned int G_LPC546xx_powerSetting[4];

void LPC546xxPowerOn(TUInt32 bits)
{
    TUInt32 index = CLK_GATE_ABSTRACT_REG_OFFSET(bits);

    G_LPC546xx_powerSetting[index] |= (1U << CLK_GATE_ABSTRACT_BITS_SHIFT(bits));

    if(index < 3){
        SYSCON->AHBCLKCTRLSET[index] = (1U << CLK_GATE_ABSTRACT_BITS_SHIFT(bits));
    } else {
        SYSCON->ASYNCAPBCTRL = SYSCON_ASYNCAPBCTRL_ENABLE(1);
        ASYNC_SYSCON->ASYNCAPBCLKCTRLSET = (1U << CLK_GATE_ABSTRACT_BITS_SHIFT(bits));
    }
}

void LPC546xxPowerOff(TUInt32 bits)
{
    TUInt32 index = CLK_GATE_ABSTRACT_REG_OFFSET(bits);

    G_LPC546xx_powerSetting[index] &= ~(1U << CLK_GATE_ABSTRACT_BITS_SHIFT(bits));

    if(index < 3){
        SYSCON->AHBCLKCTRLSET[index] &= ~(1U << CLK_GATE_ABSTRACT_BITS_SHIFT(bits));
    } else {
        SYSCON->ASYNCAPBCTRL = SYSCON_ASYNCAPBCTRL_ENABLE(1);
        ASYNC_SYSCON->ASYNCAPBCLKCTRLSET &= ~(1U << CLK_GATE_ABSTRACT_BITS_SHIFT(bits));
    }

}

void LPC546xx_ICON_ConfigPin(
        T_uezGPIOPortPin aPortPin,
        const T_LPC546xx_ICON_ConfigList *aList,
        TUInt8 aCount)
{
    while (aCount--) {
        if (aList->iPortPin == aPortPin) {
            UEZGPIOLock(aPortPin);
            UEZGPIOControl(aPortPin, GPIO_CONTROL_SET_CONFIG_BITS,
                    aList->iSetting);
            return;
        }
        aList++;
    }
    UEZFailureMsg("Bad Pin");
}

void LPC546xx_ICON_ConfigPinOrNone(
        T_uezGPIOPortPin aPortPin,
        const T_LPC546xx_ICON_ConfigList *aList,
        TUInt8 aCount)
{
    if (aPortPin == GPIO_NONE)
        return;
    while (aCount--) {
        if (aList->iPortPin == aPortPin) {
            UEZGPIOLock(aPortPin);
            UEZGPIOControl(aPortPin, GPIO_CONTROL_SET_CONFIG_BITS,
                    aList->iSetting);
            return;
        }
        aList++;
    }
    UEZFailureMsg("Bad Pin");
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC4357_UtilityFuncs.c
 *-------------------------------------------------------------------------*/

