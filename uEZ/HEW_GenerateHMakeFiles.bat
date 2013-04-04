@call CmdLineSettings.bat

cd Build\Generic\Renesas\RX62N\FreeRTOS\HEW4.x\make
start /min Generate_HMake_File.bat
sleep 200
taskkill /F /IM hew2.exe
cd ../../../../../../../

cd Build\Generic\Renesas\RX63N\FreeRTOS\HEW4.x\make
start /min Generate_HMake_File.bat
sleep 200
taskkill /F /IM hew2.exe
cd ../../../../../../../

