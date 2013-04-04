@echo off
Call ..\..\..\..\..\..\..\CmdLineSettings.bat
@echo on

"%HEWDIR%\hmake.exe" uEZ_Renesas_RX63N_FreeRTOS_HEW4.hmak
