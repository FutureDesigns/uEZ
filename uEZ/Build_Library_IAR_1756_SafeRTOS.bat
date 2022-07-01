@call CmdLineSettings.bat
%IARBUILD% "Build\Generic\NXP\LPC1756\SafeRTOS\IAR\uEZ_NXP_LPC1756_SafeRTOS_IAR.ewp" -make Release -log info
%IARBUILD% "Build\Generic\NXP\LPC1756\SafeRTOS\IAR\uEZ_NXP_LPC1756_SafeRTOS_IAR.ewp" -make Debug -log info
pause
