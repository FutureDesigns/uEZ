@echo off
@call CmdLineSettings.bat
cd Build\Generic\Renesas\RX63N\FreeRTOS\HEW4.x\make
start /min Generate_HMake_File.bat
sleep 200
taskkill /F /IM hew2.exe
@echo on
REM Building HEW HMAKE configuration: uEZ_Renesas_RX63N_FreeRTOS_HEW4
call Make.bat
cd ../../../../../../../

