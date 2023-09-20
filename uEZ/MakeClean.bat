@call CmdLineSettings.bat
%CROSSBUILD% -config Release -clean "Build\Generic\NXP\LPC1768\FreeRTOS\CrossWorks\uEZ_NXP_LPC1768_FreeRTOS_CrossWorks2.hzp" 
%CROSSBUILD% -config Debug -clean "Build\Generic\NXP\LPC1768\FreeRTOS\CrossWorks\uEZ_NXP_LPC1768_FreeRTOS_CrossWorks2.hzp"
%CROSSBUILD% -config ReleaseOptForSize -clean "Build\Generic\NXP\LPC1768\FreeRTOS\CrossWorks\uEZ_NXP_LPC1768_FreeRTOS_CrossWorks2.hzp"

%CROSSBUILD% -config Release -clean "Build\Generic\NXP\LPC1788\FreeRTOS\CrossWorks\uEZ_NXP_LPC1788_FreeRTOS_CrossWorks2.hzp" 
%CROSSBUILD% -config Debug -clean "Build\Generic\NXP\LPC1788\FreeRTOS\CrossWorks\uEZ_NXP_LPC1788_FreeRTOS_CrossWorks2.hzp"
%CROSSBUILD% -config Debug-Trace -clean "Build\Generic\NXP\LPC1788\FreeRTOS\CrossWorks\uEZ_NXP_LPC1788_FreeRTOS_CrossWorks2.hzp"
%CROSSBUILD% -config ReleaseOptForSize -clean "Build\Generic\NXP\LPC1788\FreeRTOS\CrossWorks\uEZ_NXP_LPC1788_FreeRTOS_CrossWorks2.hzp"

%CROSSBUILD% -config Release -clean "Build\Generic\NXP\LPC4088\FreeRTOS\CrossWorks\uEZ_NXP_LPC4088_FreeRTOS_CrossWorks2.hzp" 
%CROSSBUILD% -config Debug -clean "Build\Generic\NXP\LPC4088\FreeRTOS\CrossWorks\uEZ_NXP_LPC4088_FreeRTOS_CrossWorks2.hzp"
%CROSSBUILD% -config Debug-Trace -clean "Build\Generic\NXP\LPC4088\FreeRTOS\CrossWorks\uEZ_NXP_LPC4088_FreeRTOS_CrossWorks2.hzp"
%CROSSBUILD% -config ReleaseOptForSize -clean "Build\Generic\NXP\LPC4088\FreeRTOS\CrossWorks\uEZ_NXP_LPC4088_FreeRTOS_CrossWorks2.hzp"

%CROSSBUILD% -config Release -clean "Build\Generic\NXP\LPC2478\FreeRTOS\CrossWorks\uEZ_NXP_LPC2478_FreeRTOS_CrossWorks2.hzp" 
%CROSSBUILD% -config Debug -clean "Build\Generic\NXP\LPC2478\FreeRTOS\CrossWorks\uEZ_NXP_LPC2478_FreeRTOS_CrossWorks2.hzp"

%IARBUILD% "Build\Generic\NXP\LPC1756\FreeRTOS\IAR\uEZ_NXP_LPC1756_FreeRTOS_IAR.ewp" -clean Release
%IARBUILD% "Build\Generic\NXP\LPC1756\FreeRTOS\IAR\uEZ_NXP_LPC1756_FreeRTOS_IAR.ewp" -clean Debug

%IARBUILD% "Build\Generic\NXP\LPC1756\SafeRTOS\IAR\uEZ_NXP_LPC1756_SafeRTOS_IAR.ewp" -clean Release
%IARBUILD% "Build\Generic\NXP\LPC1756\SafeRTOS\IAR\uEZ_NXP_LPC1756_SafeRTOS_IAR.ewp" -clean Debug

%IARBUILD% "Build\Generic\NXP\LPC1768\FreeRTOS\IAR\uEZ_NXP_LPC1768_FreeRTOS_IAR.ewp" -clean Release
%IARBUILD% "Build\Generic\NXP\LPC1768\FreeRTOS\IAR\uEZ_NXP_LPC1768_FreeRTOS_IAR.ewp" -clean Debug

%IARBUILD% "Build\Generic\NXP\LPC1788\FreeRTOS\IAR\uEZ_NXP_LPC1788_FreeRTOS_IAR.ewp" -clean Release
%IARBUILD% "Build\Generic\NXP\LPC1788\FreeRTOS\IAR\uEZ_NXP_LPC1788_FreeRTOS_IAR.ewp" -clean Debug
%IARBUILD% "Build\Generic\NXP\LPC1788\FreeRTOS\IAR\uEZ_NXP_LPC1788_FreeRTOS_IAR.ewp" -clean Debug-Trace

%IARBUILD% "Build\Generic\NXP\LPC1788\SafeRTOS\IAR\uEZ_NXP_LPC1788_SafeRTOS_IAR.ewp" -clean Release
%IARBUILD% "Build\Generic\NXP\LPC1788\SafeRTOS\IAR\uEZ_NXP_LPC1788_SafeRTOS_IAR.ewp" -clean Debug
%IARBUILD% "Build\Generic\NXP\LPC1788\SafeRTOS\IAR\uEZ_NXP_LPC1788_SafeRTOS_IAR.ewp" -clean Debug-Trace

%IARBUILD% "Build\Generic\NXP\LPC4088\FreeRTOS\IAR\uEZ_NXP_LPC4088_FreeRTOS_IAR.ewp" -clean Release
%IARBUILD% "Build\Generic\NXP\LPC4088\FreeRTOS\IAR\uEZ_NXP_LPC4088_FreeRTOS_IAR.ewp" -clean Debug
%IARBUILD% "Build\Generic\NXP\LPC4088\FreeRTOS\IAR\uEZ_NXP_LPC4088_FreeRTOS_IAR.ewp" -clean Debug-Trace

%IARBUILD% "Build\Generic\NXP\LPC2478\FreeRTOS\IAR\uEZ_NXP_LPC2478_FreeRTOS_IAR.ewp" -clean Release
%IARBUILD% "Build\Generic\NXP\LPC2478\FreeRTOS\IAR\uEZ_NXP_LPC2478_FreeRTOS_IAR.ewp" -clean Debug

%IARBUILD% "Build\Generic\NXP\LPC2478\FreeRTOS\IAR\uEZ_NXP_LPC2478_FreeRTOS_IAR.ewp" -clean "THUMB Release"
%IARBUILD% "Build\Generic\NXP\LPC2478\FreeRTOS\IAR\uEZ_NXP_LPC2478_FreeRTOS_IAR.ewp" -clean "THUMB Debug"

RD Build\Generic\Renesas\RX62N\FreeRTOS\HEW4.x\uEZ_Renesas_RX62N_FreeRTOS_HEW4\Debug /S /Q
MKDIR Build\Generic\Renesas\RX62N\FreeRTOS\HEW4.x\uEZ_Renesas_RX62N_FreeRTOS_HEW4\Debug
RD Build\Generic\Renesas\RX62N\FreeRTOS\HEW4.x\uEZ_Renesas_RX62N_FreeRTOS_HEW4\Debug-Trace /S /Q
MKDIR Build\Generic\Renesas\RX62N\FreeRTOS\HEW4.x\uEZ_Renesas_RX62N_FreeRTOS_HEW4\Debug-Trace
RD Build\Generic\Renesas\RX62N\FreeRTOS\HEW4.x\uEZ_Renesas_RX62N_FreeRTOS_HEW4\Release /S /Q
MKDIR Build\Generic\Renesas\RX62N\FreeRTOS\HEW4.x\uEZ_Renesas_RX62N_FreeRTOS_HEW4\Release

RD Build\Generic\Renesas\RX63N\FreeRTOS\HEW4.x\uEZ_Renesas_RX63N_FreeRTOS_HEW4\Release /S /Q
MKDIR Build\Generic\Renesas\RX63N\FreeRTOS\HEW4.x\uEZ_Renesas_RX63N_FreeRTOS_HEW4\Release
RD Build\Generic\Renesas\RX63N\FreeRTOS\HEW4.x\uEZ_Renesas_RX63N_FreeRTOS_HEW4\Debug /S /Q
MKDIR Build\Generic\Renesas\RX63N\FreeRTOS\HEW4.x\uEZ_Renesas_RX63N_FreeRTOS_HEW4\Debug
RD Build\Generic\Renesas\RX63N\FreeRTOS\HEW4.x\uEZ_Renesas_RX63N_FreeRTOS_HEW4\Debug-Trace /S /Q
MKDIR Build\Generic\Renesas\RX63N\FreeRTOS\HEW4.x\uEZ_Renesas_RX63N_FreeRTOS_HEW4\Debug-Trace

RD Build\Generic\NXP\LPC1788\FreeRTOS\Keil\Release /S /Q
MKDIR Build\Generic\NXP\LPC1788\FreeRTOS\Keil\Release
RD Build\Generic\NXP\LPC1788\FreeRTOS\Keil\Debug /S /Q
MKDIR Build\Generic\NXP\LPC1788\FreeRTOS\Keil\Debug
RD Build\Generic\NXP\LPC1788\FreeRTOS\Keil\Debug-Trace /S /Q
MKDIR Build\Generic\NXP\LPC1788\FreeRTOS\Keil\Debug-Trace

RD Build\Generic\NXP\LPC4088\FreeRTOS\Keil\Release /S /Q
MKDIR Build\Generic\NXP\LPC4088\FreeRTOS\Keil\Release
RD Build\Generic\NXP\LPC4088\FreeRTOS\Keil\Debug /S /Q
MKDIR Build\Generic\NXP\LPC4088\FreeRTOS\Keil\Debug
RD Build\Generic\NXP\LPC4088\FreeRTOS\Keil\Debug-Trace /S /Q
MKDIR Build\Generic\NXP\LPC4088\FreeRTOS\Keil\Debug-Trace
