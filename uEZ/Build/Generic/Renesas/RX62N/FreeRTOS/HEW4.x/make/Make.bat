@echo off
Call ..\..\..\..\..\..\..\CmdLineSettings.bat
@echo on

"%HEWDIR%\hmake.exe" uEZ_Renesas_RX62N_FreeRTOS_HEW4.hmak
