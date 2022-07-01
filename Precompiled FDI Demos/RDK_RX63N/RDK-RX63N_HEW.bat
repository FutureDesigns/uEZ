@if not exist "RDK-RX63N_HEW.bin" goto missing
@RDK-RX63N_HEW.jlink
@goto end

:missing
@ECHO *** ERROR! ***
@ECHO Missing File "RDK-RX63N_HEW.bin"!  Cannot program unit!
@pause
@goto end

:end
