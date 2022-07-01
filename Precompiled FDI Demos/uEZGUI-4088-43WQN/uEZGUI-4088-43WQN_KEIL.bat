@if not exist "uEZGUI-4088-43WQN_KEIL_APP.bin" goto missing
@"..\JLink_V512j\JLink.exe" -jtagconf -1,-1 -autoconnect 1 -CommanderScript "uEZGUI-4088-43WQN_KEIL.jlink"
@goto end

:missing
@ECHO *** ERROR! ***
@ECHO Missing File "uEZGUI-4088-43WQN_KEIL_APP.bin"!  Cannot program unit!
@pause
@goto end

:end
