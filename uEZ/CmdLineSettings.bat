:iar_set
  if defined IARBUILD goto cross_set 
  set IARBUILD="C:\Program Files (x86)\IAR Systems\Embedded Workbench 6.30.4\common\bin\iarbuild"

:cross_set
  if defined CROSSBUILD goto keil_set
  set CROSSBUILD="C:\Program Files (x86)\Rowley Associates Limited\CrossWorks for ARM 2.2\bin\crossbuild.exe"

:keil_set
  if defined KEILUV4 goto end
  set KEILUV4="C:\Keil\UV4\UV4.exe"
  
:end
