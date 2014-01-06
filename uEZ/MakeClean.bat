@call CmdLineSettings.bat
%CROSSBUILD% -config Release -clean "Build\Generic\NXP\LPC1768\FreeRTOS\CrossWorks2.x\uEZ_NXP_LPC1768_FreeRTOS_CrossWorks2.hzp" 
%CROSSBUILD% -config Debug -clean "Build\Generic\NXP\LPC1768\FreeRTOS\CrossWorks2.x\uEZ_NXP_LPC1768_FreeRTOS_CrossWorks2.hzp"
%CROSSBUILD% -config ReleaseOptForSize -clean "Build\Generic\NXP\LPC1768\FreeRTOS\CrossWorks2.x\uEZ_NXP_LPC1768_FreeRTOS_CrossWorks2.hzp"

%CROSSBUILD% -config Release -clean "Build\Generic\NXP\LPC1788\FreeRTOS\CrossWorks2.x\uEZ_NXP_LPC1788_FreeRTOS_CrossWorks2.hzp" 
%CROSSBUILD% -config Debug -clean "Build\Generic\NXP\LPC1788\FreeRTOS\CrossWorks2.x\uEZ_NXP_LPC1788_FreeRTOS_CrossWorks2.hzp"
%CROSSBUILD% -config Debug-Trace -clean "Build\Generic\NXP\LPC1788\FreeRTOS\CrossWorks2.x\uEZ_NXP_LPC1788_FreeRTOS_CrossWorks2.hzp"
%CROSSBUILD% -config ReleaseOptForSize -clean "Build\Generic\NXP\LPC1788\FreeRTOS\CrossWorks2.x\uEZ_NXP_LPC1788_FreeRTOS_CrossWorks2.hzp"

%CROSSBUILD% -config Release -clean "Build\Generic\NXP\LPC4088\FreeRTOS\CrossWorks2.x\uEZ_NXP_LPC4088_FreeRTOS_CrossWorks2.hzp" 
%CROSSBUILD% -config Debug -clean "Build\Generic\NXP\LPC4088\FreeRTOS\CrossWorks2.x\uEZ_NXP_LPC4088_FreeRTOS_CrossWorks2.hzp"
%CROSSBUILD% -config Debug-Trace -clean "Build\Generic\NXP\LPC4088\FreeRTOS\CrossWorks2.x\uEZ_NXP_LPC4088_FreeRTOS_CrossWorks2.hzp"
%CROSSBUILD% -config ReleaseOptForSize -clean "Build\Generic\NXP\LPC4088\FreeRTOS\CrossWorks2.x\uEZ_NXP_LPC4088_FreeRTOS_CrossWorks2.hzp"

%CROSSBUILD% -config Release -clean "Build\Generic\NXP\LPC2478\FreeRTOS\CrossWorks2.x\uEZ_NXP_LPC2478_FreeRTOS_CrossWorks2.hzp" 
%CROSSBUILD% -config Debug -clean "Build\Generic\NXP\LPC2478\FreeRTOS\CrossWorks2.x\uEZ_NXP_LPC2478_FreeRTOS_CrossWorks2.hzp"

%IARBUILD% "Build\Generic\NXP\LPC1756\FreeRTOS\IAR6.x\uEZ_NXP_LPC1756_FreeRTOS_IAR6.ewp" -clean Release
%IARBUILD% "Build\Generic\NXP\LPC1756\FreeRTOS\IAR6.x\uEZ_NXP_LPC1756_FreeRTOS_IAR6.ewp" -clean Debug

%IARBUILD% "Build\Generic\NXP\LPC1756\SafeRTOS\IAR6.x\uEZ_NXP_LPC1756_SafeRTOS_IAR6.ewp" -clean Release
%IARBUILD% "Build\Generic\NXP\LPC1756\SafeRTOS\IAR6.x\uEZ_NXP_LPC1756_SafeRTOS_IAR6.ewp" -clean Debug

%IARBUILD% "Build\Generic\NXP\LPC1768\FreeRTOS\IAR6.x\uEZ_NXP_LPC1768_FreeRTOS_IAR6.ewp" -clean Release
%IARBUILD% "Build\Generic\NXP\LPC1768\FreeRTOS\IAR6.x\uEZ_NXP_LPC1768_FreeRTOS_IAR6.ewp" -clean Debug

%IARBUILD% "Build\Generic\NXP\LPC1788\FreeRTOS\IAR6.x\uEZ_NXP_LPC1788_FreeRTOS_IAR6.ewp" -clean Release
%IARBUILD% "Build\Generic\NXP\LPC1788\FreeRTOS\IAR6.x\uEZ_NXP_LPC1788_FreeRTOS_IAR6.ewp" -clean Debug
%IARBUILD% "Build\Generic\NXP\LPC1788\FreeRTOS\IAR6.x\uEZ_NXP_LPC1788_FreeRTOS_IAR6.ewp" -clean Debug-Trace

%IARBUILD% "Build\Generic\NXP\LPC1788\SafeRTOS\IAR6.x\uEZ_NXP_LPC1788_SafeRTOS_IAR6.ewp" -clean Release
%IARBUILD% "Build\Generic\NXP\LPC1788\SafeRTOS\IAR6.x\uEZ_NXP_LPC1788_SafeRTOS_IAR6.ewp" -clean Debug
%IARBUILD% "Build\Generic\NXP\LPC1788\SafeRTOS\IAR6.x\uEZ_NXP_LPC1788_SafeRTOS_IAR6.ewp" -clean Debug-Trace

%IARBUILD% "Build\Generic\NXP\LPC4088\FreeRTOS\IAR6.x\uEZ_NXP_LPC4088_FreeRTOS_IAR6.ewp" -clean Release
%IARBUILD% "Build\Generic\NXP\LPC4088\FreeRTOS\IAR6.x\uEZ_NXP_LPC4088_FreeRTOS_IAR6.ewp" -clean Debug
%IARBUILD% "Build\Generic\NXP\LPC4088\FreeRTOS\IAR6.x\uEZ_NXP_LPC4088_FreeRTOS_IAR6.ewp" -clean Debug-Trace

%IARBUILD% "Build\Generic\NXP\LPC2478\FreeRTOS\IAR6.x\uEZ_NXP_LPC2478_FreeRTOS_IAR6.ewp" -clean Release
%IARBUILD% "Build\Generic\NXP\LPC2478\FreeRTOS\IAR6.x\uEZ_NXP_LPC2478_FreeRTOS_IAR6.ewp" -clean Debug

%IARBUILD% "Build\Generic\NXP\LPC2478\FreeRTOS\IAR6.x\uEZ_NXP_LPC2478_FreeRTOS_IAR6.ewp" -clean "THUMB Release"
%IARBUILD% "Build\Generic\NXP\LPC2478\FreeRTOS\IAR6.x\uEZ_NXP_LPC2478_FreeRTOS_IAR6.ewp" -clean "THUMB Debug"

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

RD Build\Generic\NXP\LPC1788\FreeRTOS\Keil4.x\Release /S /Q
MKDIR Build\Generic\NXP\LPC1788\FreeRTOS\Keil4.x\Release
RD Build\Generic\NXP\LPC1788\FreeRTOS\Keil4.x\Debug /S /Q
MKDIR Build\Generic\NXP\LPC1788\FreeRTOS\Keil4.x\Debug
RD Build\Generic\NXP\LPC1788\FreeRTOS\Keil4.x\Debug-Trace /S /Q
MKDIR Build\Generic\NXP\LPC1788\FreeRTOS\Keil4.x\Debug-Trace

RD Build\Generic\NXP\LPC4088\FreeRTOS\Keil4.x\Release /S /Q
MKDIR Build\Generic\NXP\LPC4088\FreeRTOS\Keil4.x\Release
RD Build\Generic\NXP\LPC4088\FreeRTOS\Keil4.x\Debug /S /Q
MKDIR Build\Generic\NXP\LPC4088\FreeRTOS\Keil4.x\Debug
RD Build\Generic\NXP\LPC4088\FreeRTOS\Keil4.x\Debug-Trace /S /Q
MKDIR Build\Generic\NXP\LPC4088\FreeRTOS\Keil4.x\Debug-Trace
