@if not exist "uEZGUI-4357-50WVN_CW.bin" goto missing
@"..\JLink_V512j\JLink.exe" -jtagconf -1,-1 -autoconnect 1 -CommanderScript "uEZGUI-4357-50WVN_CW3.jlink"
@goto end

:missing
@ECHO *** ERROR! ***
@ECHO Missing File "uEZGUI-4357-50WVN_CW.bin"!  Cannot program unit!
@pause
@goto end

:end
