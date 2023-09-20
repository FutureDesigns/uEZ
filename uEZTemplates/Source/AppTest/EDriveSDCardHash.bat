E:
echo This can be used to check an SD card or USB Flash drive at driver letter E for errors after running the storage test.
echo off
for /r %%i in (TA*.txt) do (
<nul set /p ".= %%~nxi "
certutil -hashfile %%i SHA256|findstr /v "CertUtil" |findstr /v "SHA256"
)
REM certutil -hashfile TA*.TXT SHA256|findstr /v "CertUtil" > Checksums.txt 2>&1
pause

echo Check files with errors by running chkdisk. This can catch additional errors.
chkdsk E: /v

pause
pause

echo If you continue the disk will be fixed!
pause

pause
echo Fix disk
chkdsk E: /v /f

pause
