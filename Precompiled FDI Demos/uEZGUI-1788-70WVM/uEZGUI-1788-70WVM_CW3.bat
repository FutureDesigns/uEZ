@if not exist "uEZGUI-1788-70WVM_CW3.bin" goto missing
@"..\JLink_V512j\JLink.exe" -jtagconf -1,-1 -autoconnect 1 -CommanderScript "uEZGUI-1788-70WVM_CW3.jlink"
@goto end

:missing
@ECHO *** ERROR! ***
@ECHO Missing File "uEZGUI-1788-70WVM_CW3.bin"!  Cannot program unit!
@pause
@goto end

:end
