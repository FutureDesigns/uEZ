REM Generate makefile for All configurates in active project, scan dep, no static, All relative,
cd ..
"%HEWDIR%\hew2.exe" "ow uEZGUI-RX62N.hws" "cs nodebug" "gm ACCP SCAN NOSUB HMAKE ALL $(PROJECTNAME).hmak"
exit
