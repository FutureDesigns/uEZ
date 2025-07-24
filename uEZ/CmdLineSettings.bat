:iar_set
@ if defined IARBUILD goto cross_set 
@   set IARBUILD="C:\iar\ewarm-9.60.3\common\bin\iarbuild"

:cross_set
@ if defined CROSSBUILD goto keil_set
@   set CROSSBUILD="C:\Program Files\Rowley Associates Limited\CrossWorks for ARM 5.10\bin\crossbuild.exe"

:keil_set
@ if defined KEILUV4 goto hew_set
@   set KEILUV4="C:\Keil\UV5\UV5.exe"

:hew_set
@ if defined HEWDIR goto end
@   set HEWDIR=C:\Program Files\Renesas\HEW
@   rem set HEWDIR=C:\Program Files\Renesas\Hew
  
:end

@set LOGDIR=%CD%

