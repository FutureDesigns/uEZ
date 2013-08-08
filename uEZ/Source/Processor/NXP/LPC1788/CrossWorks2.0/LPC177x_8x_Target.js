/*********************************************************************
*                                                                    *
*                   (c) 2013 Future Designs Inc.                     *
*                                                                    *
*        www.teamfdi.com          Support: support@teamfdi.com       *
*                                                                    *
**********************************************************************
* Purpose : Modified extended reset support for LPC1788              *
**********************************************************************/
function Reset()
{
  TargetInterface.setNSRST(0);
  TargetInterface.delay(150);
  TargetInterface.setNSRST(1);
  TargetInterface.stop();
  TargetInterface.waitForDebugState(1000);

  TargetInterface.pokeWord(0x400FC040, 1); /* Map FLASH at 0x00000000 */
}

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

