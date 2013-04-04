REM Generate makefile for All configurates in active project, scan dep, no static, All relative,
cd ..
"%HEWDIR%\hew2.exe" "ow uEZ_Renesas_RX63N_FreeRTOS_HEW4.hws" "cs nodebug" "gm ACCP SCAN NOSUB HMAKE ALL $(PROJECTNAME).hmak"
exit
