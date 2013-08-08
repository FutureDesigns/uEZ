@call CmdLineSettings.bat
%CROSSBUILD% -config Release "Build\Generic\NXP\LPC1788\FreeRTOS\CrossWorks2.x\uEZ_NXP_LPC1788_FreeRTOS_CrossWorks2.hzp" 
%CROSSBUILD% -config Debug "Build\Generic\NXP\LPC1788\FreeRTOS\CrossWorks2.x\uEZ_NXP_LPC1788_FreeRTOS_CrossWorks2.hzp"
%CROSSBUILD% -config Debug-Trace "Build\Generic\NXP\LPC1788\FreeRTOS\CrossWorks2.x\uEZ_NXP_LPC1788_FreeRTOS_CrossWorks2.hzp"
%CROSSBUILD% -config ReleaseOptForSize "Build\Generic\NXP\LPC1788\FreeRTOS\CrossWorks2.x\uEZ_NXP_LPC1788_FreeRTOS_CrossWorks2.hzp"
pause
