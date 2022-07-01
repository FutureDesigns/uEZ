@if not exist "DK-TS-RX62N_HEW.bin" goto missing
@DK-TS-RX62N_HEW.jlink
@goto end

:missing
@ECHO *** ERROR! ***
@ECHO Missing File "DK-TS-RX62N_HEW.bin"!  Cannot program unit!
@pause
@goto end

:end
