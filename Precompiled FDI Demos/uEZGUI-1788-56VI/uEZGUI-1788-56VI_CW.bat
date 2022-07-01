@if not exist "uEZGUI-1788-56VI_CW.bin" goto missing
@uEZGUI-1788-56VI_CW.jlink
@goto end

:missing
@ECHO *** ERROR! ***
@ECHO Missing File "uEZGUI-1788-56VI_CW.bin"!  Cannot program unit!
@pause
@goto end

:end
