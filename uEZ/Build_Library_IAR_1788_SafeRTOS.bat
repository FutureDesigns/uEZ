@call CmdLineSettings.bat
%IARBUILD% "Build\Generic\NXP\LPC1788\SafeRTOS\IAR\uEZ_NXP_LPC1788_SafeRTOS_IAR.ewp" -make Release -log info
%IARBUILD% "Build\Generic\NXP\LPC1788\SafeRTOS\IAR\uEZ_NXP_LPC1788_SafeRTOS_IAR.ewp" -make Debug -log info
@rem %IARBUILD% "Build\Generic\NXP\LPC1788\SafeRTOS\IAR\uEZ_NXP_LPC1788_SafeRTOS_IAR.ewp" -make Debug-Trace -log info
pause
