@call CmdLineSettings.bat
%IARBUILD% "Build\Generic\NXP\LPC4088\FreeRTOS\IAR6.x\uEZ_NXP_LPC4088_FreeRTOS_IAR6.ewp" -make Release -log info
%IARBUILD% "Build\Generic\NXP\LPC4088\FreeRTOS\IAR6.x\uEZ_NXP_LPC4088_FreeRTOS_IAR6.ewp" -make Debug -log info
%IARBUILD% "Build\Generic\NXP\LPC4088\FreeRTOS\IAR6.x\uEZ_NXP_LPC4088_FreeRTOS_IAR6.ewp" -make Debug-Trace -log info
pause
