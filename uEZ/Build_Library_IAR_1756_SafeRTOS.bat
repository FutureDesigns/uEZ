@call CmdLineSettings.bat
%IARBUILD% "Build\Generic\NXP\LPC1756\SafeRTOS\IAR6.x\uEZ_NXP_LPC1756_SafeRTOS_IAR6.ewp" -make Release -log info
%IARBUILD% "Build\Generic\NXP\LPC1756\SafeRTOS\IAR6.x\uEZ_NXP_LPC1756_SafeRTOS_IAR6.ewp" -make Debug -log info
pause
