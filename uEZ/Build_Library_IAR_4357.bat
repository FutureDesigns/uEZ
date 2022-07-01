@call CmdLineSettings.bat
%IARBUILD% "Build\Generic\NXP\LPC4357\FreeRTOS\IAR\uEZ_NXP_LPC4357_FreeRTOS_IAR.ewp" -make Release -log info
%IARBUILD% "Build\Generic\NXP\LPC4357\FreeRTOS\IAR\uEZ_NXP_LPC4357_FreeRTOS_IAR.ewp" -make Debug -log info
%IARBUILD% "Build\Generic\NXP\LPC4357\FreeRTOS\IAR\uEZ_NXP_LPC4357_FreeRTOS_IAR.ewp" -make Debug-Trace -log info
pause
