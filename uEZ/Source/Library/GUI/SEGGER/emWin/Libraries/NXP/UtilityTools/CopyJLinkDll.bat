@rem Set the current path.
set BatchPath=%cd%
cd %BatchPath%

@copy "C:\Program Files\SEGGER\JLink\JLinkARM.dll" JLinkARM.dll
@copy "C:\Program Files\SEGGER\JLink\JLink_x64.dll" JLink_x64.dll

pause
exit
