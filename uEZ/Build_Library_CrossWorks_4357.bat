@call CmdLineSettings.bat
%CROSSBUILD3% -verbose -threadnum 4 -config Release "Build\Generic\NXP\LPC4357\FreeRTOS\CrossWorks\uEZ_NXP_LPC4357_FreeRTOS_CrossWorks.hzp" 
%CROSSBUILD3% -verbose -threadnum 4 -config Debug "Build\Generic\NXP\LPC4357\FreeRTOS\CrossWorks\uEZ_NXP_LPC4357_FreeRTOS_CrossWorks.hzp"
%CROSSBUILD3% -verbose -threadnum 4 -config Debug-Trace "Build\Generic\NXP\LPC4357\FreeRTOS\CrossWorks\uEZ_NXP_LPC4357_FreeRTOS_CrossWorks.hzp"
REM %CROSSBUILD3% -verbose -threadnum 4 -config ReleaseOptForSize "Build\Generic\NXP\LPC4357\FreeRTOS\CrossWorks\uEZ_NXP_LPC4357_FreeRTOS_CrossWorks.hzp"
REM %CROSSBUILD3% -verbose -threadnum 4 -config "Debug-Trace CLANG" "Build\Generic\NXP\LPC4357\FreeRTOS\CrossWorks\uEZ_NXP_LPC4357_FreeRTOS_CrossWorks.hzp"
pause
