// SplitLpcHexIntoBins.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "uEZCompat.h"
#include "IntelHexParser.h"

#define LPC17xx_40xx_SPIFI_ADDRESS		(0x28000000)
#define LPC17xx_40xx_NOR_ADDRESS		(0x80000000)
#define LPC43xx_SPIFI_ADDRESS			(0x14000000)
#define LPC43xx_FLASHA_ADDRESS			(0x1A000000)
#define LPC43xx_FLASHB_ADDRESS			(0x1B000000)
//#define LPC546xx_FLASH_ADDRESS		(0x00000000)
#define LPC546xx_SPIFI_ADDRESS			(0x10000000)

TUInt8 G_appMemory[512*1024];
TUInt8 G_appMemory2[512*1024];//LPC4357 has two flash banks
TUInt8 G_flashMemory[2*16*1024*1024]; // support 32MB QSPI now
TUInt32 G_appLow = 0xFFFFFFFF;
TUInt32 G_appLow2 = 0xFFFFFFFF;
TUInt32 G_flashLow = 0xFFFFFFFF;
TUInt32 G_appHigh = 0;
TUInt32 G_appHigh2 = 0;
TUInt32 G_flashHigh = 0;
TUInt32 G_FlashAddress = 0;
TUInt32 G_AppMemoryAddress = 0;
TUInt32 G_AppMemoryAddress2 = 0;

int _tmain(int argc, char* argv[])
{
    FILE *fp;
    char line[1000];
    T_IntexHexParserWorkspace parser;
    TUInt32 end;
    TUInt32 start;
	bool flashBFound = false;

    if (argc < 4 || argc > 5) {
        printf("USAGE: SplitHexIntoBins <hex file> <application .bin file> <data .bin file>\n");
		printf("Supports LPC1788, LPC4088, LPC546XX, and LPC4357 hex files\n");
		printf("LPC4357 supports and optional 5th argument for the second flash bank\n");
        return -1;
    }

    fp = fopen(argv[1], "r");
    if (!fp) {
        printf("Cannot open hex file file '%s'\n", argv[1]);
        return -1;
    }

    IntelHexParserInit(&parser);
    fgets(line, sizeof(line), fp);
    while (!feof(fp)) {
        if (IntelHexParserString(&parser, line) == UEZ_ERROR_NONE) {
            if (parser.iDataLength) {
                start = parser.iDataAddress;
                end = parser.iDataLength + parser.iDataAddress;

				switch(start & 0xFF000000){
				case 0:
                    // Must be an app memory area, LPC1788, LPC4088, LPC546XX
                    memcpy(G_appMemory+start, parser.iData, parser.iDataLength);
                    if (start < G_appLow)
                        G_appLow = start;
                    if (end > G_appHigh)
                        G_appHigh = end;
					break;
				case LPC17xx_40xx_NOR_ADDRESS:
                    // Must be an app memory area
                    G_FlashAddress = LPC17xx_40xx_NOR_ADDRESS;
                    memcpy(G_flashMemory+start-LPC17xx_40xx_NOR_ADDRESS, parser.iData, parser.iDataLength);
                    if (start < G_flashLow)
                        G_flashLow = start;
                    if (end > G_flashHigh)
                        G_flashHigh = end;
					break;
				case LPC17xx_40xx_SPIFI_ADDRESS:
					G_FlashAddress = LPC17xx_40xx_SPIFI_ADDRESS;
				    memcpy(G_flashMemory+start-LPC17xx_40xx_SPIFI_ADDRESS, parser.iData, parser.iDataLength);
                    if (start < G_flashLow)
                        G_flashLow = start;
                    if (end > G_flashHigh)
                        G_flashHigh = end;
					break;
				case LPC43xx_FLASHA_ADDRESS:
					// Must be an app memory area
                    memcpy(G_appMemory+start-LPC43xx_FLASHA_ADDRESS, parser.iData, parser.iDataLength);
                    if (start < G_appLow)
                        G_appLow = start;
                    if (end > G_appHigh)
                        G_appHigh = end;
					G_AppMemoryAddress = LPC43xx_FLASHA_ADDRESS;
					break;
				case LPC43xx_FLASHB_ADDRESS:
					// Must be an app memory area
                    memcpy(G_appMemory2+start-LPC43xx_FLASHB_ADDRESS, parser.iData, parser.iDataLength);
                    if (start < G_appLow2)
                        G_appLow2 = start;
                    if (end > G_appHigh2)
                        G_appHigh2 = end;
					flashBFound = true;
					G_AppMemoryAddress2 = LPC43xx_FLASHB_ADDRESS;
					break;
				case LPC43xx_SPIFI_ADDRESS:
					G_FlashAddress = LPC43xx_SPIFI_ADDRESS;
				    memcpy(G_flashMemory+start-LPC43xx_SPIFI_ADDRESS, parser.iData, parser.iDataLength);
                    if (start < G_flashLow)
                        G_flashLow = start;
                    if (end > G_flashHigh)
                        G_flashHigh = end;
					break;
				case LPC546xx_SPIFI_ADDRESS:
					G_FlashAddress = LPC546xx_SPIFI_ADDRESS;
					memcpy(G_flashMemory + start - LPC546xx_SPIFI_ADDRESS, parser.iData, parser.iDataLength);
					if (start < G_flashLow)
						G_flashLow = start;
					if (end > G_flashHigh)
						G_flashHigh = end;
					break;
				default:
					printf("Rogue data record found at address 0x%08lX-0x%08lX.  Skipped!\n", start, end);
					break;
				}
            }
        }
        
        fgets(line, sizeof(line), fp);
    }
    fclose(fp);

    printf("Application: 0x%08lX-0x%08lX (%d bytes)\n",
        G_appLow, G_appHigh, G_appHigh-G_appLow);
    fp = fopen(argv[2], "wb");
    if (fp) {
        fwrite(G_appMemory+G_appLow-G_AppMemoryAddress, G_appHigh-G_appLow, 1, fp);
        fclose(fp);
    } else {
        printf("Application file '%s' could not be written!\n", argv[2]);
    }

	if(argc == 5 && flashBFound){
		printf("Application: 0x%08lX-0x%08lX (%d bytes)\n",
			   G_appLow2, G_appHigh2, G_appHigh2-G_appLow2);
		fp = fopen(argv[4], "wb");
		if (fp) {
			fwrite(G_appMemory2+G_appLow2-G_AppMemoryAddress2, G_appHigh2-G_appLow2, 1, fp);
			fclose(fp);
		} else {
			printf("Application file '%s' could not be written!\n", argv[4]);
		}
	} else {
		if(flashBFound){
			printf("FlashB not specified\n");
		}
	}

    if (G_flashHigh > G_flashLow) {
        printf("Flash:       0x%08lX-0x%08lX (%d bytes)\n",
            G_flashLow, G_flashHigh, G_flashHigh-G_flashLow);
        fp = fopen(argv[3], "wb");
        if (fp) {
            fwrite(G_flashMemory+G_flashLow-G_FlashAddress, G_flashHigh-G_flashLow, 1, fp);
            fclose(fp);
        } else {
            printf("Flash file '%s' could not be written!\n", argv[3]);
        }
    }
}

