@call CmdLineSettings.bat

REM Building KEIL configuration: uEZ_NXP_LPC4357_FreeRTOS_KEIL Release
%KEILUV5% -b "Build\Generic\NXP\LPC4357\FreeRTOS\Keil\uEZ_NXP_LPC4357_FreeRTOS_KEIL.uvprojx" -t"Release" -o LOG.TXT -j0
type Build\Generic\NXP\LPC4357\FreeRTOS\Keil\LOG.TXT

REM Building KEIL configuration: uEZ_NXP_LPC4357_FreeRTOS_KEIL Debug
%KEILUV5% -b "Build\Generic\NXP\LPC4357\FreeRTOS\Keil\uEZ_NXP_LPC4357_FreeRTOS_KEIL.uvprojx" -t"Debug" -o LOG.TXT -j0
type Build\Generic\NXP\LPC4357\FreeRTOS\Keil\LOG.TXT

REM Building KEIL configuration: uEZ_NXP_LPC4357_FreeRTOS_KEIL Debug-Trace
%KEILUV5% -b "Build\Generic\NXP\LPC4357\FreeRTOS\Keil\uEZ_NXP_LPC4357_FreeRTOS_KEIL.uvprojx" -t"Debug-Trace" -o LOG.TXT -j0
type Build\Generic\NXP\LPC4357\FreeRTOS\Keil\LOG.TXT
pause
