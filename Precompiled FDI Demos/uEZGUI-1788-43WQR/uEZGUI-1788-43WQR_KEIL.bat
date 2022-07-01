@if not exist "uEZGUI-1788-43WQR_KEIL_APP.bin" goto missing
@"..\JLink_V512j\JLink.exe" -jtagconf -1,-1 -autoconnect 1 -CommanderScript "uEZGUI-1788-43WQR_KEIL.jlink"
@goto end

:missing
@ECHO *** ERROR! ***
@ECHO Missing File "uEZGUI-1788-43WQR_KEIL_APP.bin"!  Cannot program unit!
@pause
@goto end

:end
