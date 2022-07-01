@if not exist "IRD-1768_CW.bin" goto missing
@"..\JLink_V512j\JLink.exe" -jtagconf -1,-1 -autoconnect 1 -CommanderScript "IRD-1768_CW.jlink"
@goto end

:missing
@ECHO *** ERROR! ***
@ECHO Missing File "IRD-1768_CW.bin"!  Cannot program unit!
@pause
@goto end

:end
