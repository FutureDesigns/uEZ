@echo off
@call CmdLineSettings.bat
cd Build\Generic\Renesas\RX63N\FreeRTOS\HEW4.x\make
@echo on
REM Building HEW HMAKE configuration: uEZ_Renesas_RX63N_FreeRTOS_HEW4
call Make.bat
cd ../../../../../../../

pause
