@if not exist "uEZGUI-1788-70WVT_IAR_APP.bin" goto missing_app
@if not exist "uEZGUI-1788-70WVT_IAR_DATA.bin" goto missing_data
@"Install App and NOR Flash IAR.bat"
@goto end

:missing_app
@ECHO *** ERROR! ***
@ECHO Missing File "uEZGUI-1788-70WVT_IAR_APP.bin"!  Cannot program unit!
@pause
@goto end

:missing_data
@ECHO *** ERROR! ***
@ECHO Missing File "uEZGUI-1788-70WVT_IAR_DATA.bin"!  Cannot program unit!
@pause
@goto end

:end
