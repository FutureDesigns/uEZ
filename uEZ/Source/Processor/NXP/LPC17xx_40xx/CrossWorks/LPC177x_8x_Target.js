/*********************************************************************
*                                                                    *
*                   (c) 2013 Future Designs Inc.                     *
*                                                                    *
*        www.teamfdi.com          Support: support@teamfdi.com       *
*                                                                    *
**********************************************************************
* Purpose : Modified extended reset support for LPC1788              *
**********************************************************************/

/******************************************************************************
  Target Script for LPC177x/LPC178x

  Copyright (c) 2012 Rowley Associates Limited.

  This file may be distributed under the terms of the License Agreement
  provided with this software.

  THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 ******************************************************************************/

function crcIsOK()
{
  crc = 0;
  for (i = 0x00; i < 0x1C; i += 4)
    crc += TargetInterface.peekUint32(i);
  return crc == ~TargetInterface.peekUint32(0x1C) + 1;
}

function Reset()
{
  var impl = TargetInterface.implementation ? TargetInterface.implementation() : "";
  if (impl == "j-link")
    TargetInterface.resetAndStop(100);
  else if (impl != "crossworks_simulator")
    {
      // Carry out system reset 
      TargetInterface.pokeWord(0xE000EDFC, 0x00000001);
      TargetInterface.pokeWord(0xE000ED0C, 0x05FA0004);
      // Run the boot loader initialization code 
      TargetInterface.pokeWord(0xE000EDFC, 0x01000000);
      TargetInterface.pokeWord(0xE0001020, 0x00000000);
      TargetInterface.pokeWord(0xE0001024, 0x00000000);
      TargetInterface.pokeWord(0xE0001028, 0x00000005);
      TargetInterface.go();
      TargetInterface.delay(100); 
      TargetInterface.resetDebugInterface();
      TargetInterface.waitForDebugState(1000);
      if (crcIsOK())
        {
          TargetInterface.go();
          TargetInterface.waitForDebugState(1000);
        }
      TargetInterface.pokeWord(0xE0001028, 0x00000000);
    }
}

/*function Reset()
{
  TargetInterface.setNSRST(0);
  TargetInterface.delay(150);
  TargetInterface.setNSRST(1);
  TargetInterface.stop();
  TargetInterface.waitForDebugState(1000);

  TargetInterface.pokeWord(0x400FC040, 1); // Map FLASH at 0x00000000 
}*/

function SRAMReset()
{
  Reset();
}

function FlashReset()
{
  Reset();
}

function FLASHReset()
{
  Reset();
}

