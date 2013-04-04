/******************************************************************************
  Target Script for LPC1700

  Copyright (c) 2009 Rowley Associates Limited.

  This file may be distributed under the terms of the License Agreement
  provided with this software.

  THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 ******************************************************************************/

function Reset()
{
  TargetInterface.stopAndReset(1);
  TargetInterface.pokeWord(0x400FC040, 1); /* Map FLASH at 0x00000000 */
}

function SRAMReset()
{
  Reset();
}

function FLASHReset()
{
  Reset();
}


