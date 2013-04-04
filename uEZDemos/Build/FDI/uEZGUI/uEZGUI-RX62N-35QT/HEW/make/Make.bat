@echo off
Call ..\..\..\..\..\..\BatchBuild\CmdLineSettings.bat
@echo on

"%HEWDIR%\hmake.exe" uEZGUI-RX62N-35QT.hmak
