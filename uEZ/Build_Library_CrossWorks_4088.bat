@call CmdLineSettings.bat

%CROSSBUILD% -verbose -threadnum 4 -config Release "Build\Generic\NXP\LPC4088\FreeRTOS\CrossWorks\uEZ_NXP_LPC4088_FreeRTOS_CrossWorks.hzp" 
%CROSSBUILD% -verbose -threadnum 4 -config Debug "Build\Generic\NXP\LPC4088\FreeRTOS\CrossWorks\uEZ_NXP_LPC4088_FreeRTOS_CrossWorks.hzp"
%CROSSBUILD% -verbose -threadnum 4 -config Debug-Trace "Build\Generic\NXP\LPC4088\FreeRTOS\CrossWorks\uEZ_NXP_LPC4088_FreeRTOS_CrossWorks.hzp"
%CROSSBUILD% -verbose -threadnum 4 -config ReleaseOptForSize "Build\Generic\NXP\LPC4088\FreeRTOS\CrossWorks\uEZ_NXP_LPC4088_FreeRTOS_CrossWorks.hzp"
%CROSSBUILD% -verbose -threadnum 4 -config "Debug-Trace CLANG" "Build\Generic\NXP\LPC4088\FreeRTOS\CrossWorks\uEZ_NXP_LPC4088_FreeRTOS_CrossWorks.hzp"
pause
