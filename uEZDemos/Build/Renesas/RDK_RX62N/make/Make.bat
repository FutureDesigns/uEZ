@echo off
Call ..\..\..\..\BatchBuild\CmdLineSettings.bat
@echo on

"%HEWDIR%\hmake.exe" RDK_RX62N.hmak
