#include <iostream>
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include <tchar.h>

using namespace std;

#define FINE_NAME	"WVSLID"
#define COPY		0
#define IMAGE_16BIT	0

void main(int argc, char* argv[])
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	//TCHAR *directory = TEXT("c:\\test");
	TCHAR directory[MAX_PATH];
	TCHAR patter[MAX_PATH];
	char fileNames[100][MAX_PATH];
	char filePath[260];
	char command[500];
	char ch[260];
	char DefChar = ' ';
	int i = 0;
	char fileNameBase[10];
	char resolution[10];

	//cout << "Enter the File Path: ";
	//cin >> filePath;
	strcpy(filePath, argv[1]);
	strcpy(fileNameBase, argv[2]);
	strcpy(resolution, argv[3]);

	MultiByteToWideChar(CP_ACP, 0, filePath, -1, directory, MAX_PATH);
	memset(patter, 0x00, MAX_PATH);
#if COPY
	_stprintf(patter, TEXT("%s\\*.bmp"), directory);
#else
	_stprintf(patter, TEXT("%s\\*.jpg"), directory);
#endif

	hFind = FindFirstFile(patter, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		printf( "Found no files");
		return;
	} else {
		do {
			if (_tcscmp(FindFileData.cFileName, TEXT("."))==0 || _tcscmp(FindFileData.cFileName, TEXT(".."))==0){
				continue;
			}
			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
				//ignore directories
			} else {
				_tprintf(TEXT("%s\n"), FindFileData.cFileName);
				WideCharToMultiByte(CP_ACP,0,FindFileData.cFileName,-1, ch,260,&DefChar, NULL);
				strcpy(fileNames[i], ch);
				i++;
			}
		}
		while (FindNextFile(hFind, &FindFileData));	
		FindClose(hFind);
	}
	WideCharToMultiByte(CP_ACP,0,directory,-1, ch,260,&DefChar, NULL);
	strcpy(filePath, ch);

	//convert the images
	//printf(strtok(fileNames[0], "."));
#if COPY
	system("del images\\*.bmp");
#endif
	for(int j = 1; j < i +1; j++){
#if COPY
		sprintf(command, "copy %s images\\%s%02d.bmp /y",
			fileNames[j-1],
			fileNameBase,
			j);
		system(command);

#elif IMAGE_16BIT
		sprintf(command, "\"%s\\convert.exe\" "
			"\%s\\%s -resize %s -ordered-dither o8x8,32,64,32 %s\\images\\%s%02d.bmp",
			filePath,
			filePath,
			fileNames[j-1],
			resolution,
			filePath,
			fileNameBase,
			j);
		system(command);
#else
		sprintf(command, "\"%s\\convert.exe\" "
			"\%s\\%s -resize %s +matte -gravity center -extent %s -type TrueColor BMP3:%s\\images\\%s%02d.bmp",
			filePath,
			filePath,
			fileNames[j-1],
			resolution,
			resolution,
			filePath,
			fileNameBase,
			j);
		system(command);

		sprintf(command, "\"%s\\BmpCvt.exe\" %s\\images\\%s%02d.bmp -convertintobestpalette -saveas%s\\images\\%s%02d.bmp,2 -exit",
			filePath,
			filePath,
			fileNameBase,
			j,
			filePath,
			fileNameBase,
			j);
#endif
	}
	system("pause");
}
