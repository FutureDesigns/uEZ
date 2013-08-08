echo %date%, %time%: Building Libraries with IAR, CW, and HEW > BuildLog_Library.txt

REM This will clean the builds
call MakeClean.bat

REM Run each build both release and debug
call Build_Library_CrossWorks_1768.bat >> BuildLog_Library.txt 2>&1 <NUL
call Build_Library_CrossWorks_1788.bat >> BuildLog_Library.txt 2>&1 <NUL
call Build_Library_CrossWorks_2478.bat >> BuildLog_Library.txt 2>&1 <NUL

call Build_Library_IAR_1756.bat >> BuildLog_Library.txt <NUL
call Build_Library_IAR_1756_SafeRTOS.bat >> BuildLog_Library.txt <NUL
call Build_Library_IAR_1768.bat >> BuildLog_Library.txt <NUL
call Build_Library_IAR_1788.bat >> BuildLog_Library.txt <NUL
call Build_Library_IAR_1788_SafeRTOS.bat >> BuildLog_Library.txt <NUL
call Build_Library_IAR_2478.bat >> BuildLog_Library.txt <NUL
call Build_Library_IAR_2478_Thumb.bat >> BuildLog_Library.txt <NUL

call Build_Library_HEW_RX62N.bat >> BuildLog_Library.txt <NUL
call Build_Library_HEW_RX63N.bat >> BuildLog_Library.txt <NUL

call Build_Library_Keil_1788.bat >> BuildLog_Library.txt <NUL

REM Results are in BuildLog_Library.txt
