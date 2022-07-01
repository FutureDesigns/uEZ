@if not exist "uEZGUI-RX62N-35QT_HEW.bin" goto missing
@uEZGUI-RX62N-35QT_HEW.jlink
@goto end

:missing
@ECHO *** ERROR! ***
@ECHO Missing File "uEZGUI-RX62N-35QT_HEW.bin"!  Cannot program unit!
@pause
@goto end

:end
