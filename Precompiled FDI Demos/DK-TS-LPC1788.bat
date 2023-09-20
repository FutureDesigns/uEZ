@ECHO off
"C:\Program Files\SEGGER\JLink\JLink.exe" -CommanderScript "DK-TS-LPC1788.jlink" > JLink.log.txt
REM JLink.exe only has two values for ERRORLEVEL: 0 and 1
REM see https://wiki.segger.com/J-Link_Commander#JLink.exe_return_value

IF ERRORLEVEL 1 goto ERROR
goto END
:ERROR
ECHO Error programing unit!
ECHO
ECHO Check that power is properly applied to the unit
ECHO Check that the Segger J-Link is connected to the USB Port
ECHO     and the ready light is solid Green
ECHO Check that the Segger J-Link is connected to the unit
ECHO
more JLink.log.txt
pause
exit 1
:END
exit 0
