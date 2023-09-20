echo off
REM Tool to Configure/Program GS Module
if [%1]==[-h] goto HELP
if [%1]==[/?] goto HELP
if [%1]==[] goto HELP
REM if [%8]==[GSWEBP.txt] goto PART3

:START
echo.
echo. GS1011 Module configuration script ver 2.5 
echo. ------------------------------------------
echo.

echo.
echo.
echo. Part1
echo. ----- 
echo. Put the board in PROGRAM mode to read MAC address
REM pause.
mode COM%1  BAUD=115200 PARITY=n DATA=8
echo GAINSPANPROGRAM > \\.\COM%1 
REM pause
:PART1
echo. Reading MAC Address....

echo. Verify MAC at x.4.x location...
gs_flashprogram dump 0x801f800 8 -f macID -S%1
gs_checksum.exe macID.bin
if errorlevel 1 goto :transfermacID 
if errorlevel 0 goto :storemacID

:transfermacID
echo.
echo. Check x.3.x location...
REM pause.
:transfermacID
gs_flashprogram dump 0x801e800 8 -f macID -S%1 
gs_checksum.exe macID.bin
if errorlevel 1 goto :errormessage
if errorlevel 0 goto :storemacID

:errormessage
echo.
echo. Invalid MAC Address. Program with a valid MAC address which is on the label.
echo. 
REM goto :END
@echo off
echo. Enter the MAC Address: 
set /p mac_address= 
echo.
echo. Mac Address is: %mac_address%
echo.
gs_flashprogram -e0 -e1 -ew -S%1 -v
echo.
echo. Program MAC address at x_4_x loaction
gs_flashprogram.exe -S%1 -G4 -m %mac_address% -v
echo.
goto :PART1

:storemacID
echo. gs_facdef facDef.txt facDflttemp.bin
gs_facdef facDef.txt facDflttemp.bin
copy /b macID.bin + facDflttemp.bin  facDflt.bin
echo. Part1 done !
echo.

echo.
echo.
echo. Part2
echo. ----- 
echo. Keep the board in PROGRAM mode and continue with the erasing of APP0, APP1, WLAN Firmwares
REM pause.
:PART2
echo. Performing a full chip erase.....
echo. gs_flashprogram -e0 -e1 -ew -S%1
gs_flashprogram -e0 -e1 -ew -S%1 
echo.


echo. Program custom WLAN binary (WFW_GS1011_RECOVERY_WD_DISABLE.bin)
echo. gs_flashprogram -w WFW_GS1011_RECOVERY_WD_DISABLE.bin -S%1 -v
gs_flashprogram -w WFW_GS1011_RECOVERY_WD_DISABLE.bin -S%1 -v
echo. Part2 done !
echo.

echo.
echo. Part3
echo. ----- 
echo. Power cycle the board.
pause.
echo Now that the board has been power cycled, it will be put into Run mode
pause
mode COM%1 BAUD=115200 PARITY=n DATA=8
echo GAINSPANRUNMODE > \\.\COM%1
REM pause
:PART3
echo. Erasing External Flash chip...
echo. gs_sfp sfe -S%1 
gs_sfp sfe -S%1 
echo. Part3 done !
echo.


echo.
echo. Part4
echo. ----- 
echo. Keep the board in RUN mode
echo. Programming External Flash (super info block, MAC CB and MAC bin and Web pages in external flash)....

REM pause.

echo.
echo. Write sf_info.bin file (pls, provide full path)...
echo. gs_sfp sfw -f %2 0x0000 -S%1 
gs_sfp sfw -f %2 0x0000 -S%1 
echo.

echo.
echo. Build Web file from a Root directory (pls, provide full path)...
echo. gs_sfp fs_tool -d %3 -f %4 -p 256 -v
REM gs_sfp fs_tool -d %3 -f %4 -p 256 
echo.

echo.
echo. Programming Web file....
echo. gs_sfp sfw -f %4 0xf0000 -S%1 -v
REM gs_sfp sfw -f %4 0xf0000 -S%1 -v
gs_sfp sfw_verify -f ExternalFlash.bin 0xf0000 -S%1 -v
echo. Part4 done !
echo.

echo.
echo. Part5
echo. -----
echo. Power cycle the board and put it in PROGRAM mode
echo. Programming APP0, APP1....
pause.
mode COM%1 BAUD=115200 PARITY=n DATA=8
echo GAINSPANPROGRAM > \\.\COM%1
REM pause
:PART5
echo. gs_flashprogram -w %5 -0 %6 -1 %7 -S%1 
gs_flashprogram -e0 -e1 -ew -S%1 
echo.
gs_flashprogram -w %5 -0 %6 -1 %7 -S%1 
echo. Part5 done !
echo.

echo.
echo. Part6
echo. -----
echo. Restoring the MAC address
REM pause.
:PART6
echo.
echo. gs_flashprogram -S%1 -a 0x0801F800:facDflt.bin
gs_flashprogram -S%1 -a 0x0801F800:facDflt.bin
echo. Part6 done !
echo.

echo. Now power cycle the board
pause
echo. Now that the board is power cycled the webserver will be activated to test the preprogrammed webpage.
pause
mode COM%1  BAUD=115200 PARITY=n DATA=8
echo GAINSPANRUNMODE > \\.\COM%1 
mode COM%1  BAUD=115200 PARITY=n DATA=8
echo AT+NSET=192.168.3.1,255.255.255.0,192.168.3.1 > \\.\COM%1 
mode COM%1  BAUD=115200 PARITY=n DATA=8
echo AT+WM=2 > \\.\COM%1 
mode COM%1  BAUD=115200 PARITY=n DATA=8
echo AT+DHCPSRVR=1 > \\.\COM%1 
mode COM%1  BAUD=115200 PARITY=n DATA=8
echo AT+WEBPROV=admin,admin,0,2,,1 > \\.\COM%1 
mode COM%1  BAUD=115200 PARITY=n DATA=8
echo AT+WA=GainSpanCustom,,11 > \\.\COM%1 

echo. Now wait a few seconds for the wireless to activate, then connect to the new network
pause
192.168.3.1.url

goto END

:HELP
echo.
echo. Script for testing configuration/testing GS module
echo. Requirements:
echo.               External Port must be defined
echo.               gs_flashprogram.exe and gs_sfp.exe must be in the script directory or environment
echo.               parameter (path) must be set
echo.               pls, provide full path to file/directory inputs
echo.
REM                          %1            %2                  %3                      %4               %5               %6              %7
echo. Usage: gs_config_ext (port) (sf_info_file.bin) (WebPages Root_Directory) (Web_Out_file.bin) (WLAN_file.bin) (APP0_file.bin) (App1_file.bin)
echo.
echo. EXAMPLE: gs_config_ext 1 G:\sf_info.bin G:\webpage_directory G:\Out.bin G:\WFW-GS1500-REL-2_4_1.bin G:\s2w-2_4_1-app1.bin G:\s2w-2_4_1-app2.bin
echo.

:END
echo. Configuration script finished
pause
