:iar_set
@ if defined IARBUILD goto cross_set 
@   set IARBUILD="C:\Program Files\IAR Systems\Embedded Workbench 8.11\common\bin\iarbuild"

:cross_set
@ if defined CROSSBUILD goto keil_set
@   set CROSSBUILD=""C:\Program Files\Rowley Associates Limited\CrossWorks for ARM 4.0\bin\crossbuild.exe""

:keil_set
@ if defined KEILUV4 goto hew_set
@   set KEILUV4="C:\Keil\UV5\UV5.exe"

:hew_set
@ if defined HEWDIR goto end
@   set HEWDIR=C:\Program Files\Renesas\HEW
@   rem set HEWDIR=C:\Program Files\Renesas\Hew
  
:end

@set LOGDIR=%CD%

