@if not exist "uEZGUI-4357-50WVN_IAR.bin" goto missing
REM @if not exist "uEZGUI-4357-50WVN_IAR_DATA.bin" goto missing_data
@"..\JLink_V612a\JLink.exe" -jtagconf -1,-1 -autoconnect 1 -CommanderScript "uEZGUI-4357-50WVN_IAR.jlink"
@goto end

:missing
@ECHO *** ERROR! ***
@ECHO Missing File "uEZGUI-4357-50WVN_IAR.bin"!  Cannot program unit!
@pause
@goto end

:missing_data
@ECHO *** ERROR! ***
@ECHO Missing File "uEZGUI-4088-43WQN_IAR_DATA.bin"!  Cannot program unit!
@pause
@goto end

:end
