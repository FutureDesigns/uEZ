@echo off
Call ..\..\..\..\..\..\..\BatchBuild\CmdLineSettings.bat
@echo on

REM Fix fwrite warning.
C:\GnuWin32\bin\sed -c -i s/nomessage=1320/nomessage/g RDK_RX63N.hmak
C:\GnuWin32\bin\sed -c -i 0,/nomessage/{s/nomessage/nomessage=1320/g} RDK_RX63N.hmak

del sed*

"%HEWDIR%\hmake.exe" RDK_RX63N.hmak