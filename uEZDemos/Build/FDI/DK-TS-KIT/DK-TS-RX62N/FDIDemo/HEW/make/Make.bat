@echo off
Call ..\..\..\..\..\..\..\BatchBuild\CmdLineSettings.bat
@echo on

"%HEWDIR%\hmake.exe" DK-TS-RX62N.hmak
