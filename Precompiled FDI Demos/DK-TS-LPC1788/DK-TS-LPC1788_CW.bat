@if not exist "DK-TS-LPC1788_CW.bin" goto missing
@DK-TS-LPC1788_CW.jlink
@goto end

:missing
@ECHO *** ERROR! ***
@ECHO Missing File "DK-TS-LPC1788_CW.bin"!  Cannot program unit!
@pause
@goto end

:end
