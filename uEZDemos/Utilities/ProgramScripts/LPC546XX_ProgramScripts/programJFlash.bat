echo off 
"C:\Program Files (x86)\SEGGER\JLink\JFlash.exe" -openprj"LPC54608.jflash" -open"LPC54608.hex" -erasechip -auto -startapp -exit

IF ERRORLEVEL 1 goto ERROR
goto END
:ERROR
ECHO Error programing unit!
ECHO
ECHO Check that power is properly applied to the unit
ECHO Check that the Segger J-Link is connected to the PC
ECHO     and the ready light is solid Green
ECHO Check that the Segger J-Link is connected to the unit
ECHO     If in a fixture check that the connection to the fixture
ECHO     is properly interest
ECHO If problems persist, insect the area of the board around U1 and Y2
ECHO
more JFlashARM.log
pause
exit 1
:END
exit 0
