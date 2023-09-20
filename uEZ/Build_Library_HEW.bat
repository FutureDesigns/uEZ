echo %date%, %time%: Building Libraries with HEW > BuildLog_Library.txt

REM This will clean the builds
call MakeClean.bat

REM Run each build both release and debug
call Build_Library_HEW_RX62N.bat >> BuildLog_Library.txt
call Build_Library_HEW_RX63N.bat >> BuildLog_Library.txt

REM Results are in BuildLog_Library.txt
pause
