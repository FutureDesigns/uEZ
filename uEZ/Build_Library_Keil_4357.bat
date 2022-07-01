@call CmdLineSettings.bat

REM Building KEIL configuration: uEZ_NXP_LPC4357_FreeRTOS_KEIL Release
%KEILUV4% -b "Build\Generic\NXP\LPC4357\FreeRTOS\Keil\uEZ_NXP_LPC4357_FreeRTOS_KEIL.uvproj" -t"Release" -o LOG.TXT -j0
type Build\Generic\NXP\LPC4357\FreeRTOS\Keil\LOG.TXT

REM Building KEIL configuration: uEZ_NXP_LPC4357_FreeRTOS_KEIL Debug
%KEILUV4% -b "Build\Generic\NXP\LPC4357\FreeRTOS\Keil\uEZ_NXP_LPC4357_FreeRTOS_KEIL.uvproj" -t"Debug" -o LOG.TXT -j0
type Build\Generic\NXP\LPC4357\FreeRTOS\Keil\LOG.TXT

REM Building KEIL configuration: uEZ_NXP_LPC4357_FreeRTOS_KEIL Debug-Trace
%KEILUV4% -b "Build\Generic\NXP\LPC4357\FreeRTOS\Keil\uEZ_NXP_LPC4357_FreeRTOS_KEIL.uvproj" -t"Debug-Trace" -o LOG.TXT -j0
type Build\Generic\NXP\LPC4357\FreeRTOS\Keil\LOG.TXT
pause
