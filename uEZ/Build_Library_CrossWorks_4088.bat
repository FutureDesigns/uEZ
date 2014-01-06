@call CmdLineSettings.bat
%CROSSBUILD% -config Release "Build\Generic\NXP\LPC4088\FreeRTOS\CrossWorks2.x\uEZ_NXP_LPC4088_FreeRTOS_CrossWorks2.hzp" 
%CROSSBUILD% -config Debug "Build\Generic\NXP\LPC4088\FreeRTOS\CrossWorks2.x\uEZ_NXP_LPC4088_FreeRTOS_CrossWorks2.hzp"
%CROSSBUILD% -config Debug-Trace "Build\Generic\NXP\LPC4088\FreeRTOS\CrossWorks2.x\uEZ_NXP_LPC4088_FreeRTOS_CrossWorks2.hzp"
%CROSSBUILD% -config ReleaseOptForSize "Build\Generic\NXP\LPC4088\FreeRTOS\CrossWorks2.x\uEZ_NXP_LPC4088_FreeRTOS_CrossWorks2.hzp"
pause
