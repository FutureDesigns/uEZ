@call CmdLineSettings.bat
%CROSSBUILD% -verbose -threadnum 4 -config Release "Build\Generic\NXP\LPC1768\FreeRTOS\CrossWorks\uEZ_NXP_LPC1768_FreeRTOS_CrossWorks.hzp" 
%CROSSBUILD% -verbose -threadnum 4 -config Debug "Build\Generic\NXP\LPC1768\FreeRTOS\CrossWorks\uEZ_NXP_LPC1768_FreeRTOS_CrossWorks.hzp"
%CROSSBUILD% -verbose -threadnum 4 -config ReleaseOptForSize "Build\Generic\NXP\LPC1768\FreeRTOS\CrossWorks\uEZ_NXP_LPC1768_FreeRTOS_CrossWorks.hzp"
pause
