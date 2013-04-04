@echo off
@call CmdLineSettings.bat
cd Build\Generic\Renesas\RX62N\FreeRTOS\HEW4.x\make
start /min Generate_HMake_File.bat
sleep 200
taskkill /F /IM hew2.exe
@echo on
REM Building HEW HMAKE configuration: uEZ_Renesas_RX62N_FreeRTOS_HEW4
call Make.bat
cd ../../../../../../../

