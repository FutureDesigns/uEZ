@if not exist "uEZGUI-1788-70WVM_KEIL_APP.bin" goto missing
@uEZGUI-1788-70WVM_KEIL.jlink
@goto end

:missing
@ECHO *** ERROR! ***
@ECHO Missing File "uEZGUI-1788-70WVM_KEIL_APP.bin"!  Cannot program unit!
@pause
@goto end

:end
