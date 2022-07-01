@echo off
"..\JLink_V480h\JLink.exe" -jtagconf -1,-1 -autoconnect 1 -CommanderScript "NORProgram.jlink
echo Wait for the message "** NOR Flash Programming Complete. **"
"..\JLink_V480h\JLink.exe" -jtagconf -1,-1 -autoconnect 1 -CommanderScript "ProgramAPP.jlink
echo Programming of App and NOR flash complete!
