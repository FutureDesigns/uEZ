@if not exist "IRD-1768_IAR_APP.bin" goto missing_app
@"..\JLink_V480h\JLink.exe" -CommanderScript "IRD-1768_IAR_APP.jlink"
@goto end

:missing_app
@ECHO *** ERROR! ***
@ECHO Missing File "IRD-1768_IAR_APP.bin"!  Cannot program unit!
@pause
@goto end

:end
