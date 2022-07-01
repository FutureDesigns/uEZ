@call CmdLineSettings.bat
%CROSSBUILD% -config Release "Build\Generic\NXP\LPC1768\FreeRTOS\CrossWorks\uEZ_NXP_LPC1768_FreeRTOS_CrossWorks.hzp" 
%CROSSBUILD% -config Debug "Build\Generic\NXP\LPC1768\FreeRTOS\CrossWorks\uEZ_NXP_LPC1768_FreeRTOS_CrossWorks.hzp"
%CROSSBUILD% -config ReleaseOptForSize "Build\Generic\NXP\LPC1768\FreeRTOS\CrossWorks2.x\uEZ_NXP_LPC1768_FreeRTOS_CrossWorks.hzp"
pause
