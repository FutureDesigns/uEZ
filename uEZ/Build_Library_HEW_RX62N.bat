@echo off
@call CmdLineSettings.bat
cd Build\Generic\Renesas\RX62N\FreeRTOS\HEW4.x\make
@echo on
REM Building HEW HMAKE configuration: uEZ_Renesas_RX62N_FreeRTOS_HEW4
call Make.bat
cd ../../../../../../../

pause
