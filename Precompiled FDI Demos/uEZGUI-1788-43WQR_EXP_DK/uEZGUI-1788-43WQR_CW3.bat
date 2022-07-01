@if not exist "uEZGUI-1788-43WQR_CW3.bin" goto missing
@uEZGUI-1788-43WQR_CW3.jlink
@goto end

:missing
@ECHO *** ERROR! ***
@ECHO Missing File "uEZGUI-1788-43WQR_CW3.bin"!  Cannot program unit!
@pause
@goto end

:end
