@if not exist "RDK-RX62N_HEW.bin" goto missing
@RDK-RX62N_HEW.jlink
@goto end

:missing
@ECHO *** ERROR! ***
@ECHO Missing File "RDK-RX62N_HEW.bin"!  Cannot program unit!
@pause
@goto end

:end
