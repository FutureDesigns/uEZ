@call CmdLineSettings.bat
%IARBUILD% "Build\Generic\NXP\LPC1768\FreeRTOS\IAR6.x\uEZ_NXP_LPC1768_FreeRTOS_IAR6.ewp" -make Release -log info
%IARBUILD% "Build\Generic\NXP\LPC1768\FreeRTOS\IAR6.x\uEZ_NXP_LPC1768_FreeRTOS_IAR6.ewp" -make Debug -log info
pause
