@call CmdLineSettings.bat
%IARBUILD% "Build\Generic\NXP\LPC2478\FreeRTOS\IAR6.x\uEZ_NXP_LPC2478_FreeRTOS_IAR6.ewp" -make Release -log info
%IARBUILD% "Build\Generic\NXP\LPC2478\FreeRTOS\IAR6.x\uEZ_NXP_LPC2478_FreeRTOS_IAR6.ewp" -make Debug -log info
pause
