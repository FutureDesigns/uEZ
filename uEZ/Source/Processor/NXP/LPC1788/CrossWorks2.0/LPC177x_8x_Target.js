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
  if (TargetInterface.implementation != undefined)
    {
      if (TargetInterface.implementation() == "crossworks_hardware")
        {
          /* Run the boot loader initialization code */
          TargetInterface.pokeWord(0xE000EDFC, 0x01000000);
          TargetInterface.pokeWord(0xE0001020, 0x00000000);
          TargetInterface.pokeWord(0xE0001024, 0x00000000);
          TargetInterface.pokeWord(0xE0001028, 0x00000805);
          TargetInterface.go();
          TargetInterface.delay(100);
          TargetInterface.resetDebugInterface();
          TargetInterface.waitForDebugState(1000);
          TargetInterface.pokeWord(0xE0001028, 0x00000000);
        }
    }
  else
    {
      TargetInterface.pokeWord(0x400FC040, 1); /* Map FLASH at 0x00000000 */
    }
}

function SRAMReset()
{
  Reset();
}

function FLASHReset()
{
  Reset();
}


