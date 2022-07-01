echo %date%, %time%: Building Libraries with IAR and CW > BuildLog_Library.txt

REM This will clean the builds
call MakeClean.bat

REM Run each build both release and debug
call Build_Library_CrossWorks_1768.bat >> BuildLog_Library.txt 2>&1 <NUL
call Build_Library_CrossWorks_1788.bat >> BuildLog_Library.txt 2>&1 <NUL
call Build_Library_CrossWorks_4088.bat >> BuildLog_Library.txt 2>&1 <NUL
call Build_Library_CrossWorks_4357.bat >> BuildLog_Library.txt 2>&1 <NUL
call Build_Library_CrossWorks_456X.bat >> BuildLog_Library.txt 2>&1 <NUL

call Build_Library_IAR_1756.bat >> BuildLog_Library.txt <NUL
call Build_Library_IAR_1756_SafeRTOS.bat >> BuildLog_Library.txt <NUL
call Build_Library_IAR_1768.bat >> BuildLog_Library.txt <NUL
call Build_Library_IAR_1788.bat >> BuildLog_Library.txt <NUL
call Build_Library_IAR_1788_SafeRTOS.bat >> BuildLog_Library.txt <NUL
call Build_Library_IAR_4088.bat >> BuildLog_Library.txt <NUL
call Build_Library_IAR_4357.bat >> BuildLog_Library.txt <NUL
call Build_Library_IAR_456X.bat >> BuildLog_Library.txt <NUL

REM Results are in BuildLog_Library.txt
