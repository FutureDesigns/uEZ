#!/bin/sh
# /opt/SEGGER/JLink is default symlink dir of new SEGGER releases
/opt/SEGGER/JLink/JLinkExe -CommanderScript "LPC54608.jlink"
if [ $? = "0" ]
 then
	exit 0
else	
	echo "Error programing unit!" 1>&2
	echo
	echo Check that power is properly applied to the unit
	echo Check that the Segger J-Link is connected to the USB HUB
	echo     and the ready light is solid Green
	echo Check that the Segger J-Link is connected to the unit
	echo
	cat JLink.log
	exit 1
fi
