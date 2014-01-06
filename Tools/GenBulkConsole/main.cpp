#include <windows.h>

#include "usb.h"
#include <stdio.h>
#include "GenBulkStream.h"

HANDLE hStdout, hStdin;

int main(void)
{
    char buffer[512];
    int num;
	CHAR c;
	CONSOLE_CURSOR_INFO cinfo;
	DWORD cNumRead, i; 
	INPUT_RECORD irInBuf[128]; 
	int sayLooking = 1;
	int error = 0;

	hStdin = GetStdHandle(STD_INPUT_HANDLE); 
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE); 

    GenBulkStream s;
	SetConsoleMode(hStdin, ENABLE_PROCESSED_OUTPUT|ENABLE_PROCESSED_INPUT);
	GetConsoleCursorInfo(hStdin, &cinfo);
	cinfo.bVisible = TRUE;
	SetConsoleCursorInfo(hStdin, &cinfo);
	SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
	printf("GenBulkConsole v1.00\n\n");
	
	while (1) {
		if (sayLooking) {
			SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
			printf("Looking for device ...\n\n");
			sayLooking = 0;
		}
		if (s.Open()) {
			SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
			printf("Connected ...\n\n");
			//s.Write("PING\r\n", 6);
			while (1) {
				num = s.Read(buffer, sizeof(buffer));
				//num = 0;
				if (num > 0) {
					buffer[num] = '\0';
					SetConsoleTextAttribute(hStdout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
					printf("%s", buffer);
				}

				if (! PeekConsoleInput( 
					hStdin,      // input buffer handle 
					irInBuf,     // buffer to read into 
					1,			// size of read buffer 
					&cNumRead) ) // number of records read 
					exit(1);
				if (cNumRead) {
					if (! ReadConsoleInput( 
						hStdin,      // input buffer handle 
						irInBuf,     // buffer to read into 
						128,         // size of read buffer 
						&cNumRead) ) // number of records read 
						exit(1);
 
					// Dispatch the events to the appropriate handler. 
					for (i = 0; i < cNumRead; i++) 
					{
						if (irInBuf[i].EventType == KEY_EVENT) {
								//KeyEventProc(irInBuf[i].Event.KeyEvent); 
								if (irInBuf[i].Event.KeyEvent.bKeyDown) {
									if ((irInBuf[i].Event.KeyEvent.uChar.AsciiChar)) {
										SetConsoleTextAttribute(hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
										c = irInBuf[i].Event.KeyEvent.uChar.AsciiChar;
	#if 0
										if (c == '\r') {
											s.Write("\r\n", 2);
											printf("\r\n");
										} if (c == '\b') {
											s.Write(&c, 1);
											printf("\b \b");
										} else {
											s.Write(&c, 1);
											printf("%c", c);
										}
	#else
										s.Write(&c, 1);
	#endif
									}
								}
						} 
					}
				} else {
					Sleep(10);
				}
				if (s.GetErrorCode() != 0) {
					s.Close();
					SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
					printf("Disconnected!\n");
					sayLooking = 1;
					break;
				}
			}  
		}
		Sleep(100);
	}
    return 0;
}
