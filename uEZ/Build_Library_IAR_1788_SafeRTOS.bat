@call CmdLineSettings.bat
%IARBUILD% "Build\Generic\NXP\LPC1788\SafeRTOS\IAR6.x\uEZ_NXP_LPC1788_SafeRTOS_IAR6.ewp" -make Release -log info
%IARBUILD% "Build\Generic\NXP\LPC1788\SafeRTOS\IAR6.x\uEZ_NXP_LPC1788_SafeRTOS_IAR6.ewp" -make Debug -log info
@rem %IARBUILD% "Build\Generic\NXP\LPC1788\SafeRTOS\IAR6.x\uEZ_NXP_LPC1788_SafeRTOS_IAR6.ewp" -make Debug-Trace -log info
pause
