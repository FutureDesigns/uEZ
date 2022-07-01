@if not exist "DK-TS-LPC1788_KEIL_APP.bin" goto missing
@DK-TS-LPC1788_KEIL.jlink
@goto end

:missing
@ECHO *** ERROR! ***
@ECHO Missing File "DK-TS-LPC1788_KEIL_APP.bin"!  Cannot program unit!
@pause
@goto end

:end
