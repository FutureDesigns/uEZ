@call CmdLineSettings.bat
%IARBUILD% "Build\Generic\NXP\LPC1756\FreeRTOS\IAR\uEZ_NXP_LPC1756_FreeRTOS_IAR.ewp" -make Release -log info
%IARBUILD% "Build\Generic\NXP\LPC1756\FreeRTOS\IAR\uEZ_NXP_LPC1756_FreeRTOS_IAR.ewp" -make Debug -log info
pause
