@echo off
echo Convert Video to image files.
echo Set the below 5 parameters for chosen video file.

set VIDEO_FPS=10
set VIDEO_WIDTH=800
set VIDEO_HEIGHT=480
set VIDEO_OUTPUT_NAME=FDI
set VIDEO_FILE_NAME=Video.mp4

@echo on
ffmpeg -i "%VIDEO_FILE_NAME%" -s %VIDEO_WIDTH%x%VIDEO_HEIGHT% -r %VIDEO_FPS% "VID%%05d.TGA"
@echo off

echo Rip audio from Video file, scale to 22050Hz mono.
@echo on

ffmpeg -i "%VIDEO_FILE_NAME%" -map a:0 -ac 1 -ar 22050 AUDIO.WAV
@echo off

set cnt=0
for %%A in (*.TGA) do set /a cnt+=1
echo Total number of video frames = %cnt%.

echo Now Converting TGA files to the uncompressed format for uEZGUI video, this could take a very long time to complete for long videos!
for %%f in (*.TGA) do convert  %%f -compress None %%f

echo Create Video file out of images.

VideoTGAtoBIN.exe
echo VideoTGAtoBIN.exe 5550 "VID%%05d.TGA" VIDEO.BIN 1 %cnt% %VIDEO_WIDTH% %VIDEO_HEIGHT%
VideoTGAtoBIN.exe 5550 "VID%%05d.TGA" VIDEO.BIN 1 %cnt% %VIDEO_WIDTH% %VIDEO_HEIGHT%

echo Move files into separate Output video Directory.

@rd /S /Q OUTPUT
mkdir OUTPUT
move VIDEO.BIN OUTPUT/%VIDEO_OUTPUT_NAME%.BIN
move AUDIO.WAV OUTPUT/%VIDEO_OUTPUT_NAME%.WAV

echo Delete TGA files as they are no longer needed.
DEL VID*.TGA

echo The final video is inside of the OUTPUT folder directory.
echo You can now copy this folder onto the uEZGUI's microSD card's VIDEOS folder.
echo Next rename the folder to your choice (8 characters max) then add the new entry to the video player VIDEOS.ini file.
echo Press any key to exit...
pause
