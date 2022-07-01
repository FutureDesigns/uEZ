@if not exist "DK-TS-LPC1788_IAR_APP.bin" goto missing_app
@"DK-TS-LPC1788_IAR.jlink"
@goto end

:missing_app
@ECHO *** ERROR! ***
@ECHO Missing File "DK-TS-LPC1788_IAR_APP.bin"!  Cannot program unit!
@pause
@goto end

:end
