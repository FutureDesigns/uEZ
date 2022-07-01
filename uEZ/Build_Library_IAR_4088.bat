@call CmdLineSettings.bat
%IARBUILD% "Build\Generic\NXP\LPC4088\FreeRTOS\IAR\uEZ_NXP_LPC4088_FreeRTOS_IAR.ewp" -make Release -log info
%IARBUILD% "Build\Generic\NXP\LPC4088\FreeRTOS\IAR\uEZ_NXP_LPC4088_FreeRTOS_IAR.ewp" -make Debug -log info
%IARBUILD% "Build\Generic\NXP\LPC4088\FreeRTOS\IAR\uEZ_NXP_LPC4088_FreeRTOS_IAR.ewp" -make Debug-Trace -log info
pause
