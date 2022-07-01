@if not exist "DK-TS-RX63N_HEW.bin" goto missing
@DK-TS-RX63N_HEW.jlink
@goto end

:missing
@ECHO *** ERROR! ***
@ECHO Missing File "DK-TS-RX63N_HEW.bin"!  Cannot program unit!
@pause
@goto end

:end
