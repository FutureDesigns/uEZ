@call CmdLineSettings.bat
%IARBUILD% "Build\Generic\NXP\LPC1788\FreeRTOS\IAR\uEZ_NXP_LPC1788_FreeRTOS_IAR.ewp" -make Release -log info
%IARBUILD% "Build\Generic\NXP\LPC1788\FreeRTOS\IAR\uEZ_NXP_LPC1788_FreeRTOS_IAR.ewp" -make Debug -log info
%IARBUILD% "Build\Generic\NXP\LPC1788\FreeRTOS\IAR\uEZ_NXP_LPC1788_FreeRTOS_IAR.ewp" -make Debug-Trace -log info
pause
