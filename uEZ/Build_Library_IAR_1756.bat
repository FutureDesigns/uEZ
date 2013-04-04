@call CmdLineSettings.bat
%IARBUILD% "Build\Generic\NXP\LPC1756\FreeRTOS\IAR6.x\uEZ_NXP_LPC1756_FreeRTOS_IAR6.ewp" -make Release -log info
%IARBUILD% "Build\Generic\NXP\LPC1756\FreeRTOS\IAR6.x\uEZ_NXP_LPC1756_FreeRTOS_IAR6.ewp" -make Debug -log info