@call CmdLineSettings.bat
%IARBUILD% "Build\Generic\NXP\LPC1768\FreeRTOS\IAR\uEZ_NXP_LPC1768_FreeRTOS_IAR.ewp" -make Release -log info
%IARBUILD% "Build\Generic\NXP\LPC1768\FreeRTOS\IAR\uEZ_NXP_LPC1768_FreeRTOS_IAR.ewp" -make Debug -log info
pause
