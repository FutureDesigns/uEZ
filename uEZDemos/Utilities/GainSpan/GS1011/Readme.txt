This is a programming script for performing the full external flash programming with the GS1011 Gainspan modules in UART programming mode.
This script is designed for batch programming of the GainSpan modules after they have been loaded onto the uEZGUI module.
For testing new GainSpan software it is recommened to use the GUI program tool.
GainSpan now has a custom firmware generation utility to let you build the firmware with the exact features you need.
In addition a custom webpage can be stored in the Wi-Fi serial flash. (or a webserver can be run from the uEZGUI)
Using the Gainspan provided web pages and external flash chip, over the air firmware updates can be performed for the Gainspan module.

The file gs_config_ext-prebuiltwebpage.bat is the script. It needs to be called with parameters to speficy the firmware file names, COM port to use, etc.

Program.bat calls this script with parameters.
The example is:
call gs_config_ext-prebuiltwebpage.bat 12 infoblock.bin webpages ExternalFlash.bin WFW-REL-2_5_1.bin s2w-2_5_1-app1.bin s2w-2_5_1-app2.bin 2.5
where 
12 is the COM port number for COM12
infoblock.bin is the info block file.
webpages is the directory if building a custom webpage from a directory
ExternalFlash.bin is the pre-compiled external flash image
WFW-REL-2_5_1.bin is the wireless firmware
s2w-2_5_1-app1.bin is part one of the application binary
s2w-2_5_1-app2.bin is part two of the application binary
2.5 is the version number to program into the software.

This script must be used with the gsflashprogram_DOS_ver_7_16_final.zip download from GainSpan's website.
Just copy the script and custom firmware/webpage/external flash files all into the same directory with the programming executables on a Windows machine.
Connect a serial port to a uEZGUI either through RS232 on the uEZGUI-EXP1, or through the ALT PWR COM connector using a USB to TTL adapter.
FTDI based USB chip adapters are recommended.

Normally on Gainspan developer kits there is a phsyical switch to enable run or program mode. Obviously this cannot be done on a typical uEZGUI configuration.
With uEZ 2.07 new console commands were added to enable putting the unit into either Gainspan programming, or Gainspan run mode.
In this way the programming can all take place using just a serial port and the passthrough mode in software.
The script will call the commands "GAINSPANPROGRAM" and "GAINSPANRUNMODE" to switch between modes.
The uEZGUI will need to be reset by the operator between modes, either by power cycling the unit or the reset button on the unit.
This script has been designed for uEZGUI units such as the uEZGUI-1788-56VI-BA, uEZGUI-1788-70WVT-BA, and the uEZGUI-1788-70WVM-BA with a GainSpan footprint available.
For other configurations, the uEZGUI will need a customized platform file function to control the UART and GPIO pins going into the GainSpan module.

This script is slightly modified from the GainSpan provided script. Please see the documenation available on GainSpan's website for help or more information about the programming process.