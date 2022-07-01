@if not exist "uEZGUI-4088-43WQN_CW.bin" goto missing
@uEZGUI-4088-43WQN_CW.jlink
@goto end

:missing
@ECHO *** ERROR! ***
@ECHO Missing File "uEZGUI-4088-43WQN_CW.bin"!  Cannot program unit!
@pause
@goto end

:end
