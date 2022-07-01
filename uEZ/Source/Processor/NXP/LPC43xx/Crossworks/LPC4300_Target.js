/******************************************************************************
  Target Script for NXP LPC4300

  Copyright (c) 2012 Rowley Associates Limited.

  This file may be distributed under the terms of the License Agreement
  provided with this software.

  THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 ******************************************************************************/

function EnableTrace(TraceInterfaceType)
{
  if (TraceInterfaceType == "ETB")
    {
      TargetInterface.pokeWord(0x40043128, 0); // Enable ETB
      TargetInterface.pokeWord(0xE0040004, 0x00000008); // TPIU_CSPSR
      TargetInterface.pokeWord(0xE00400F0, 0x00000000); // TPIU_SPPR
    }
  else if (TraceInterfaceType == "TracePort")
    {
      TargetInterface.pokeWord(0x40086790, 0x22);
      TargetInterface.pokeWord(0x40086794, 0x23);
      TargetInterface.pokeWord(0x40086798, 0x23);
      TargetInterface.pokeWord(0x4008679C, 0x23);
      TargetInterface.pokeWord(0x400867A0, 0x23);
    }
  else if (TraceInterfaceType == "SWO")
    {
    }
}

function ResetCM0()
{
  TargetInterface.pokeWord(0xE000EDFC, 0x1); // set VC_CORERESET in DEMCR
  if (TargetInterface.peekWord(0x40053154) & (1<<24)) // (RESET_ACTIVE_STATUS1 & M0APP_RST) - CM0 not in reset
    TargetInterface.pokeWord(0x40053104, (1<<24)); // RESET_CTRL = M0APP_RST - set CM0 reset          
  TargetInterface.pokeWord(0x40053104, 0); // RESET_CTRL = 0 - clear CM0 reset
  TargetInterface.waitForDebugState(1000);
}

function ResetCM0SUB()
{
  TargetInterface.pokeWord(0xE000EDFC, 0x1); // set VC_CORERESET in DEMCR
  if (TargetInterface.peekWord(0x40053150) & (1<<12)) // (RESET_ACTIVE_STATUS1 & M0SUB_RST) - CM0 not in reset
    TargetInterface.pokeWord(0x40053100, (1<<12)); // RESET_CTRL = M0SUB_RST - set CM0 reset          
  TargetInterface.pokeWord(0x40053100, 0); // RESET_CTRL = 0 - clear CM0 reset
  TargetInterface.waitForDebugState(1000);
}

function ResetCM4()
{
  TargetInterface.pokeWord(0xE000EDF0, 0xA05F0003); // set C_HALT and C_DEBUGEN in DHCSR
  TargetInterface.waitForDebugState(1000);   

  TargetInterface.pokeWord(0x4004312C, TargetInterface.peekWord(0x4004312C)|(1<<17)); // Power up RNG
  TargetInterface.pokeWord(0x40043100, 0x10400000); // set M4MEMMAP to bootloader

  TargetInterface.pokeWord(0xE000EDFC, 0x1); // set VC_CORERESET in DEMCR
  TargetInterface.pokeWord(0xE000ED0C, 0x05FA0004); // set SYSRESETREQ in AIRCR 
  TargetInterface.waitForDebugState(1000);  

   // The boot ROM hasn't run - if it had then...
  // PLL1 to 96Mhz
  TargetInterface.pokeWord(0x40050044, 0x01170881);
  TargetInterface.pokeWord(0x40050044, 0x01170880);
  // Feed PLL1 into IDIVC
  TargetInterface.pokeWord(0x40050050, 0x09000808);
  // Run the Base M3 clock off IDIVC
  TargetInterface.pokeWord(0x4005006C, 0x0e000800);
}

function Reset(core)
{
  if (TargetInterface.implementation() == "j-link")
    {
      TargetInterface.resetAndStop(100);
      return;
    }
  if (core==0)
    ResetCM0();
  else if (core==4)
    ResetCM4();
  else if (core==1)
    ResetCM0SUB();
}

function scu_pinmux(port, pin, mode, func)
{
  TargetInterface.pokeWord(0x40086000+(0x80*port+0x4*pin), mode+func);
}

function SPIFIReset(core)
{
  if (TargetInterface.implementation() == "j-link")
    {
      TargetInterface.resetAndStop(100);
      return;
    }
  Reset(core);
  TargetInterface.pokeBinary(0x10000000, "$(TargetsDir)/LPC4300/Release/SPIFI_Init.bin");
  TargetInterface.runFromAddress(0x10000001, 1000);
}

function IdentifyPart()
{
  var partName = "Unknown";
  switch (TargetInterface.peekWord(0x40045000))
    {
      case 0xA0000850:
      case 0xA0000830:
        partName = "LPC4350";
        break;
      case 0xA0000860:
        partName = "LPC43S50";
        break;
      case 0xA0000A30:
        partName = "LPC4330";
        break;
      case 0xA0000A60:
        partName = "LPC43S30";
        break;
      case 0xA0008B3C:
      case 0xA000CB3C:
        partName = "LPC4320";
        break;
      case 0xA000CB6C:
        partName = "LPC43S20";
        break;
      case 0xA00ACB3F:
        partName = "LPC4310";
        break;
      case 0x00000030:
      case 0x00000230:
        partName = "LPC4370";
        break;
      case 0x00000060:
        partName = "LPC43S70";
        break;
      case 0x8001C030:
        partName = "LPC4367";
        break;
      case 0x8001C060:
        partName = "LPC43S67";
        break;
      case 0xA001C830:
        switch (TargetInterface.peekWord(0x4004500C) & 0xff)
          {
            case 0x00:
              partName = "LPC4357";
              break;
            case 0x44:
              partName = "LPC4353";
              break;
          }
        break;
      case 0xA001C860:
        partName = "LPC43S57";
        break;
      case 0xA001CA30:
        switch (TargetInterface.peekWord(0x4004500C) & 0xff)
          {
            case 0x00:
              partName = "LPC4337";
              break;
            case 0x44:
              partName = "LPC4333";
              break;
          }
        break;
      case 0xA001CA60:
        partName = "LPC43S37";
        break;
      case 0xA001CB3C:
        switch (TargetInterface.peekWord(0x4004500C) & 0xff)
          {
            case 0x00:
              partName = "LPC4327";
              break;
            case 0x22:
              partName = "LPC4325";
              break;
          }
        break;
      case 0xA00BCB3C:
        switch (TargetInterface.peekWord(0x4004500C) & 0xff)
          {
            case 0x44:
              partName = "LPC4323";
              break;
            case 0x80:
              partName = "LPC4322";
              break;
          }
        break;
      case 0xA001CB3F:
        switch (TargetInterface.peekWord(0x4004500C) & 0xff)
          {
            case 0x00:
              partName = "LPC4317";
              break;
            case 0x22:
              partName = "LPC4315";
              break;
          }
        break;
      case 0xA00BCB3F:
        switch (TargetInterface.peekWord(0x4004500C) & 0xff)
          {
            case 0x44:
              partName = "LPC4313";
              break;
            case 0x80:
              partName = "LPC4312";
              break;
          }
        break;
    }  
  return partName;
}

function GetPartName()
{ 
  var partName = IdentifyPart();
  var CPUID = TargetInterface.peekWord(0xE000ED00);
  if (((CPUID>>4)&0xf)==0) // Cortex-M0
    partName += "_M0";
  else
    {
      partName += "_M4";
      TargetInterface.pokeWord(0x40043128, 0); // Enable ETB
    }
  return partName;
}

function MatchPartName(name)
{
  return IdentifyPart().substring(0, name.length)==name;
}
