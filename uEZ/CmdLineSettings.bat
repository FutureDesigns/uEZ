:iar_set
@ if defined IARBUILD goto cross_set 
@   rem set IARBUILD="C:\Program Files\IAR Systems\Embedded Workbench 6.30.4\common\bin\iarbuild"
@   set IARBUILD="C:\Program Files\IAR Systems\Embedded Workbench 6.30.4\common\bin\iarbuild"

:cross_set
@ if defined CROSSBUILD goto keil_set
@   rem set CROSSBUILD="C:\Program Files\Rowley Associates Limited\CrossWorks for ARM 2.2\bin\crossbuild.exe"
@   set CROSSBUILD="C:\Program Files\Rowley Associates Limited\CrossWorks for ARM 2.2\bin\crossbuild.exe"
@   rem set CROSSBUILD="C:\Apps\Rowley Associates Limited\CrossWorks for ARM 2.0\bin\crossbuild.exe"

:keil_set
@ if defined KEILUV4 goto hew_set
@   set KEILUV4="C:\Keil\UV4\UV4.exe"

:hew_set
@ if defined HEWDIR goto end
@   set HEWDIR=C:\Program Files\Renesas\HEW
@   rem set HEWDIR=C:\Program Files\Renesas\Hew
  
:end

@set LOGDIR=%CD%

