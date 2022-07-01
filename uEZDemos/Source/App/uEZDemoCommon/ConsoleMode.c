/*-------------------------------------------------------------------------*
 * File:  ConsoleMode.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Put a console on the screen and allow TCP/IP to connect and
 *      control the device.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZLCD.h>
#include <uEZFile.h>
#include <Source/Library/Graphics/SWIM/lpc_rom8x8.h>
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>
#include <stdio.h>  				
#include <string.h>
#include <sys/types.h>  				
#include <sys/socket.h>  				
#include <netinet/in.h>  				
#include <netdb.h>
#include "uEZDemoCommon.h"
#include <uEZPlatform.h>
#include <uEZLCD.h>
#include <uEZKeypad.h>

/*-------------------------------------------------------------------------*
 * External Hooks:
 *-------------------------------------------------------------------------*/
extern void BeepError(void);

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#if UEZ_DEFAULT_LCD_RES_QVGA
    #define CO_COLUMN_1    13
    #define CO_COLUMN_2    98
    #define CO_BUTTON_HEIGHT  32
    #define CO_BUTTON_WIDTH   64
    #define CO_ROW_HEIGHT   52
    #define CO_ROWS_TOP     9
    #define CO_IMAGE_WIDTH  208
    #define CO_IMAGE_HEIGHT 200
    #define CO_IMAGE_LEFT   95
    #define CONSOLE_HEIGHT  25
    #define CONSOLE_WIDTH   26

#endif

#if (UEZ_DEFAULT_LCD_RES_VGA || UEZ_DEFAULT_LCD_RES_WVGA)
    #define CO_COLUMN_1    26
    #define CO_COLUMN_2    186
    #define CO_BUTTON_HEIGHT  64
    #define CO_BUTTON_WIDTH   128
    #define CO_ROW_HEIGHT   104
    #define CO_ROWS_TOP     18

    #define CO_IMAGE_WIDTH  416
    #define CO_IMAGE_HEIGHT 400
    #define CO_IMAGE_LEFT   190
    #define CONSOLE_HEIGHT  50
    #define CONSOLE_WIDTH   51
#endif

#if (UEZ_DEFAULT_LCD==LCD_RES_480x272)
    #define CO_COLUMN_1    17
    #define CO_COLUMN_2    98
    #define CO_BUTTON_HEIGHT  32
    #define CO_BUTTON_WIDTH   64
    #define CO_ROW_HEIGHT   52
    #define CO_ROWS_TOP     9+16

    #define CO_IMAGE_WIDTH  360
    #define CO_IMAGE_HEIGHT 200
    #define CO_IMAGE_LEFT   103
    #define CONSOLE_HEIGHT  25
    #define CONSOLE_WIDTH   45
#endif

#define CO_ROW_1       (CO_ROWS_TOP+(CO_ROW_HEIGHT*0))
#define CO_ROW_2       (CO_ROWS_TOP+(CO_ROW_HEIGHT*1))
#define CO_ROW_3       (CO_ROWS_TOP+(CO_ROW_HEIGHT*2))
#define CO_ROW_4       (CO_ROWS_TOP+(CO_ROW_HEIGHT*3))
#define CO_IMAGE_RIGHT  (CO_IMAGE_LEFT+CO_IMAGE_WIDTH+2)
#define CO_IMAGE_TOP    CO_ROWS_TOP
#define CO_IMAGE_BOTTOM (CO_IMAGE_TOP+CO_IMAGE_HEIGHT+2)

#define WPSPORT 23  	 /* server port number */  			

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void CO_Exit(const T_choice *aChoice);

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static SWIM_WINDOW_T G_coWin;
static const T_choice G_coChoices[] = {
    { CO_COLUMN_1, CO_ROW_4, CO_COLUMN_1+CO_BUTTON_WIDTH-1, CO_ROW_4+CO_BUTTON_HEIGHT-1, "Exit", CO_Exit, G_exitIcon },
    { 0, 0, 0, 0, 0 },
};

static TUInt8 G_coChars[CONSOLE_HEIGHT][CONSOLE_WIDTH];
static TUInt8 G_coCharsLast[CONSOLE_HEIGHT][CONSOLE_WIDTH];
static TUInt16 G_coCursorX, G_coCursorY;
static TBool G_coModified = EFalse;
static T_uezSemaphore G_coSem = 0;
static TBool G_coExit;

#if (UEZ_ENABLE_TCPIP_STACK)
static int ClntDescriptor;  /* socket descriptor to client */  			
static int SrvrDescriptor;  /* socket descriptor for server */  			

static TBool G_tcpServerRunning = EFalse;
#endif

/*---------------------------------------------------------------------------*
 * Routine:  CO_DrawChars
 *---------------------------------------------------------------------------*
 * Description:
 *      Update and draw the changed console characters on the screen.
 *---------------------------------------------------------------------------*/
void CO_DrawChars(void)
{
    TUInt8 x, y;

    if (G_coSem) {
        UEZSemaphoreGrab(G_coSem, UEZ_TIMEOUT_INFINITE);

        swim_set_font(&G_coWin, &font_rom8x8);
        swim_set_font_transparency(&G_coWin, 0);
        swim_set_bkg_color(&G_coWin, BLACK);
        swim_set_pen_color(&G_coWin, GREEN);

        for (y=0; y<CONSOLE_HEIGHT; y++) {
            for (x=0; x<CONSOLE_WIDTH; x++) {
                if (G_coChars[y][x] != G_coCharsLast[y][x]) {
                    swim_set_xy(
                        &G_coWin,
                        x*8+CO_IMAGE_LEFT+2,
                        y*8+CO_IMAGE_TOP+2);
                    swim_put_char(&G_coWin, G_coChars[y][x]);
                    G_coCharsLast[y][x] = G_coChars[y][x];
                }
            }
        }

        swim_set_font_transparency(&G_coWin, 1);
        G_coModified = EFalse;

        UEZSemaphoreRelease(G_coSem);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  CO_ScrollUp
 *---------------------------------------------------------------------------*
 * Description:
 *      Scroll up the console one line
 *---------------------------------------------------------------------------*/
void CO_ScrollUp(void)
{
    TUInt32 y;

    // Copy all the characters up a line
    for (y=1; y<CONSOLE_HEIGHT; y++) {
        memcpy(G_coChars[y-1], G_coChars[y], CONSOLE_WIDTH);
    }
    memset(G_coChars[CONSOLE_HEIGHT-1], ' ', CONSOLE_WIDTH);
}

/*---------------------------------------------------------------------------*
 * Routine:  CO_AddChar
 *---------------------------------------------------------------------------*
 * Description:
 *      Add a character to the console while also handling special control
 *      characters such as line feed, carriage return, form feed, and
 *      backspace.
 * Inputs:
 *      char c                    -- Character to add.
 *---------------------------------------------------------------------------*/
void CO_AddChar(char c)
{
    if (G_coSem) {
        UEZSemaphoreGrab(G_coSem, UEZ_TIMEOUT_INFINITE);
        if (G_coCursorX >= CONSOLE_WIDTH)
            G_coCursorX = 0;
        if (G_coCursorY >= CONSOLE_HEIGHT)
            G_coCursorY = 0;

        if (c == '\f') {
            memset(G_coChars, ' ', sizeof(G_coChars));
            G_coCursorX = 0;
            G_coCursorY = 0;
        } else if (c == '\n') {
            G_coCursorY++;
            if (G_coCursorY >= CONSOLE_HEIGHT) {
                CO_ScrollUp();
                G_coCursorY--;
            }
        } else if (c == '\r') {
            G_coCursorX = 0;
        } else if (c == '\b') {
            if (G_coCursorX > 0) {
                G_coCursorX--;
                G_coChars[G_coCursorY][G_coCursorX] = ' ';
            }
        } else {
            G_coChars[G_coCursorY][G_coCursorX] = c;
            G_coCursorX++;
            if (G_coCursorX >= CONSOLE_WIDTH) {
                G_coCursorY++;
                if (G_coCursorY >= CONSOLE_HEIGHT) {
                    CO_ScrollUp();
                    G_coCursorY--;
                }
            }
        }
        G_coModified = ETrue;
        UEZSemaphoreRelease(G_coSem);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  CO_AddString
 *---------------------------------------------------------------------------*
 * Description:
 *      Add a whole string of characters to the console.
 * Inputs:
 *      const char *aString         -- String to add
 *---------------------------------------------------------------------------*/
void CO_AddString(const char *aString)
{
    while (*aString)
        CO_AddChar(*(aString++));
}

/*---------------------------------------------------------------------------*
 * Routine:  CO_Screen
 *---------------------------------------------------------------------------*
 * Description:
 *      Draw the console screen with terminal and exit button.
 * Inputs:
 *      T_uezDevice lcd             -- LCD to draw screen on
 *---------------------------------------------------------------------------*/
void CO_Screen(T_uezDevice lcd)
{
    T_pixelColor *pixels;
    int c;
    const char *p;
    TUInt32 width;
    INT_32 imageX, imageY;
    const T_choice *p_choice;

    SUIHidePage0();

    UEZLCDGetFrame(lcd, 0, (void **)&pixels);
    swim_window_open(
        &G_coWin,
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT,
        pixels,
        0,
        0,
        DISPLAY_WIDTH-1,
        DISPLAY_HEIGHT-1,
        2,
        YELLOW,
        RGB(0, 0, 0),
        RED);
    swim_set_font(&G_coWin, &APP_DEMO_DEFAULT_FONT);
    swim_set_title(&G_coWin, "uEZ(tm) Ethernet Console Demonstration", BLUE);
    swim_set_pen_color(&G_coWin, YELLOW);

    for (c=0; G_coChoices[c].iText; c++) {
        p_choice = G_coChoices+c;
        if (p_choice->iIcon) {
            imageX = p_choice->iLeft;
            imageY = p_choice->iTop;
            swim_get_physical_xy(&G_coWin, &imageX, &imageY);
            SUIDrawIcon(
                p_choice->iIcon,
                imageX,
                imageY);
        } else {
            swim_put_box(&G_coWin,
                p_choice->iLeft,
                p_choice->iTop,
                p_choice->iRight,
                p_choice->iBottom);
        }

        // Calculate width of text
        width = 0;
        for (p=G_coChoices[c].iText; *p; p++)
            width += swim_get_font_char_width(&G_coWin, *p);

        // Cut width in half
        width >>= 1;

        // Now draw centered below.
        swim_put_text_xy(
            &G_coWin,
            G_coChoices[c].iText,
            ((G_coChoices[c].iLeft + G_coChoices[c].iRight)>>1)-width,
            G_coChoices[c].iBottom+2);
    }

    // Now draw the drawing area
    swim_set_fill_color(
        &G_coWin,
        BLACK);
    swim_put_box(
        &G_coWin,
        CO_IMAGE_LEFT,
        CO_IMAGE_TOP,
        CO_IMAGE_RIGHT,
        CO_IMAGE_BOTTOM);

//    swim_put_line(&G_coWin, CO_IMAGE_LEFT, CO_IMAGE_TOP, CO_IMAGE_RIGHT, CO_IMAGE_BOTTOM);
//    swim_put_line(&G_coWin, CO_IMAGE_RIGHT, CO_IMAGE_BOTTOM, CO_IMAGE_LEFT, CO_IMAGE_TOP);
    CO_DrawChars();

    SUIShowPage0();
}

/*---------------------------------------------------------------------------*
 * Routine:  CO_Exit
 *---------------------------------------------------------------------------*
 * Description:
 *      User is choosing to exit the console mode screen.
 * Inputs:
 *      const T_choice *aChoice     -- Choice used to get here
 *---------------------------------------------------------------------------*/
void CO_Exit(const T_choice *aChoice)
{
    G_coExit = 1;
}

/*---------------------------------------------------------------------------*
 * Task:  BasicTCPServer
 *---------------------------------------------------------------------------*
 * Description:
 *      Task used to run the tcp server.  It sits and waits for connections
 *      and handles commands.  When session ends, it will wait again.
 *      Only allows one session at a time.
 * Inputs:
 *      T_uezTask aMyTask           -- Task handle
 *      void *aParams               -- Parameters to use with task.
 *---------------------------------------------------------------------------*/
#if (UEZ_ENABLE_TCPIP_STACK)
static TUInt32 BasicTCPServer(T_uezTask aMyTask, void *aParams)
{  	
    struct sockaddr_in BindInfo;
    struct sockaddr_in rem;
    char c;
    unsigned int len;		
    char command[80];
    char line[80];
    int clen=0;
    TBool forever = ETrue;
    TBool needPrompt = EFalse;
    const char banner[] = PROJECT_NAME " " VERSION_AS_TEXT "\r\n";

    /* create an Internet TCP socket */  			
    SrvrDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    /* bind it to port 2000 (> 1023, to avoid the  			
    "well-known ports"), allowing connections from  			
    any NIC */  			
    BindInfo.sin_family = AF_INET;  			
    BindInfo.sin_port = htons(WPSPORT);
    BindInfo.sin_addr.s_addr = INADDR_ANY;  			
    bind(SrvrDescriptor, (struct sockaddr *)&BindInfo, sizeof(BindInfo));

    /* OK, set queue length for client calls */  			
    listen(SrvrDescriptor,5);  			

    while (forever){  			
        CO_AddString("\r\n\r\nWaiting...\r\n");			
        /* wait for a call */  			
        len = sizeof(rem);
        ClntDescriptor = accept(SrvrDescriptor,(struct sockaddr*)&rem, (u32_t *) &len);			
        CO_AddString("Connected!\r\n");
        needPrompt = ETrue;

        CO_AddString(banner);
        write(ClntDescriptor, banner, sizeof(banner)-1);
        while (1)  {
            if (needPrompt) {
                write(ClntDescriptor, "> ", 2);
                CO_AddString("> ");
                needPrompt = EFalse;
            }
            if (read(ClntDescriptor, &c, 1) <= 0)
                break;
//            write(ClntDescriptor, &c, 1);
            CO_AddChar(c);
            if (c == '\r') {
            } else if (c == '\n') {
                if (strcmp(command, "quit") == 0) {
                    closesocket(ClntDescriptor);
                } else if (strcmp(command, "dir") == 0) {
                    T_uezFileEntry entry;

                    if (UEZFileFindStart(CONSOLE_ROOT_DIRECTORY, &entry) == UEZ_ERROR_NONE) {
                        do {
                            sprintf(line, "%c %-12.12s %02d/%02d/%04d\r\n",
                                (entry.iFileAttributes & UEZ_FILE_ENTRY_ATTR_DIRECTORY)?'D':'F',
                                entry.iFilename,
                                entry.iModifiedDate.iDay,
                                entry.iModifiedDate.iMonth,
                                entry.iModifiedDate.iYear
                                );
                            CO_AddString(line);
                            write(ClntDescriptor, line, strlen(line));
                        } while (UEZFileFindNext(&entry) == UEZ_ERROR_NONE);
                    } else {
                        printf("No files found.\n");
                    }
                    UEZFileFindStop(&entry);
                } else {
                    CO_AddString("\r\nUnknown cmd!\r\n");
                    write(ClntDescriptor, "\r\nUnknown cmd!\r\n", 16);
                }
                needPrompt = ETrue;
                clen = 0;
                command[0] = '\0';
            } else if (c == '\b') {
                if (clen > 0) {
                    clen--;
                    command[clen] = '\0';
                }
            } else {
                if (clen < sizeof(command)-1)
                    command[clen++] = c;
                command[clen] = '\0';
            }
        }
        CO_AddString("Closed.\r\n");			
        close(ClntDescriptor);
    }
    return 0;
} 	 			
#endif /* UEZ_ENABLE_TCPIP_STACK */

/*---------------------------------------------------------------------------*
 * Routine:  ConsoleMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Main mode being run while console is running.
 *      Brings up the console server if not already running and
 *      then sits and shows characters on the screen until exit icon
 *      chosen.
 * Inputs:
 *      const T_choice *aChoice     -- Choice used to start this mode
 *---------------------------------------------------------------------------*/
void ConsoleMode(const T_choice *aChoice)
{
    T_uezDevice lcd;
    T_uezDevice ts;
    static T_uezQueue queue = (TUInt32)NULL;
    T_pixelColor *pixels;
#if UEZ_ENABLE_BUTTON_BOARD
    T_uezDevice keypadDevice;
#endif
    
    memset(G_coCharsLast, 0, sizeof(G_coCharsLast));
    memset(G_coChars, ' ', sizeof(G_coChars));
    G_coCursorX = G_coCursorY = 0;

#ifdef NO_DYNAMIC_MEMORY_ALLOC	
	if (NULL == queue)
	{
	  	if (UEZQueueCreate(1, sizeof(T_uezInputEvent), &queue) != UEZ_ERROR_NONE)
		{
		  	queue = NULL;
		}
	}
	
    if (NULL != queue) {
		/* Register the queue so that the IAR Stateviewer Plugin knows about it. */
	  	UEZQueueAddToRegistry( queue, "Console TS" );	
#else
	if (UEZQueueCreate(1, sizeof(T_uezInputEvent), &queue) == UEZ_ERROR_NONE) {
#if UEZ_REGISTER
        UEZQueueSetName(queue, "Console", "\0");
#endif
#endif
#if UEZ_ENABLE_BUTTON_BOARD
        UEZKeypadOpen("BBKeypad", &keypadDevice, &queue);
#endif
        // Open up the touchscreen and pass in the queue to receive events
        if (UEZTSOpen("Touchscreen", &ts, &queue)==UEZ_ERROR_NONE)  {
            // Open the LCD and get the pixel buffer
            if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
                G_coExit = EFalse;

                if (G_coSem == 0)
                    UEZSemaphoreCreateBinary(&G_coSem);

#if (UEZ_ENABLE_TCPIP_STACK)
                if (1){
                    if (!G_tcpServerRunning) {
                        if (UEZTaskCreate(BasicTCPServer, "TCPServ", UEZ_TASK_STACK_BYTES(2048), (void *)0, UEZ_PRIORITY_NORMAL, 0))
                            return;
                        G_tcpServerRunning = ETrue;

                        CO_AddString("Telnet to port 23\r\n");
                    }
#else				
					/* TODO - can we link to serial terminal ? */
                if (0){
#endif
                    UEZLCDGetFrame(lcd, 0, (void **)&pixels);

                    // Put the draw screen up
                    CO_Screen(lcd);

                    while (!G_coExit) {
                        // Wait forever until we receive a touchscreen event
                        // NOTE: UEZTSGetReading() can also be used, but it doesn't wait.
                        ChoicesUpdate(&G_coWin, G_coChoices, queue, UEZ_TIMEOUT_INFINITE);

                        if (G_coModified)
                            CO_DrawChars();
                    }
                } else {
                    CO_Screen(lcd);
                    CO_AddString("Network not enabled!");
                    CO_DrawChars();
                    BeepError();
                    UEZTaskDelay(2000);
                }

                UEZLCDClose(lcd);
            }
            UEZTSClose(ts, queue);
        }
#if UEZ_ENABLE_BUTTON_BOARD
        UEZKeypadClose(keypadDevice, &queue);
#endif
#ifndef NO_DYNAMIC_MEMORY_ALLOC	
        UEZQueueDelete(queue);
#endif
    }
}

/*-------------------------------------------------------------------------*
 * File:  ConsoleMode.c
 *-------------------------------------------------------------------------*/
