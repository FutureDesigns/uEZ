// VideoTGAtoBIN565.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <io.h>

// Go ahead and increase size to support 1920x1080 video input, even though we most certainly won't ever use it.
// Future software will switch to JPEG decoding.
#define MAX_VIDEO_WIDTH 1920//800
#define MAX_VIDEO_HEIGHT 1080//480
#define WORK_MEMORY 256 // About double what we seem to need.

// One of these buffers needs additional ~128KB of work memory on top of the 32bit frame memory, so go ahead and add that to the calculation.
char G_imageTGA[MAX_VIDEO_WIDTH*MAX_VIDEO_HEIGHT*2+(WORK_MEMORY*1024)];
char G_uncompressedTGA[MAX_VIDEO_WIDTH*MAX_VIDEO_HEIGHT*2+(WORK_MEMORY*1024)];

unsigned char G_memory[MAX_VIDEO_WIDTH*MAX_VIDEO_HEIGHT*2+(WORK_MEMORY*1024)];

int G_VideoHeight;
int G_VideoWidth;

// 555 version required for 4357 video player as apparently the 565 doesn't work currently.
void render5550(void)
{
    char* p_pixel = G_imageTGA + 18;
    unsigned int p, r, g, b;
    unsigned int m = 0;
    int x, y;
    for (y = 0; y < G_VideoHeight; y++) {
        p_pixel = G_imageTGA + 18 + ((G_VideoHeight - 1 - y) * G_VideoWidth * 3);
        for (x = 0; x < G_VideoWidth; x++, p_pixel += 3) {
            b = (p_pixel[0] & 0xF8) >> 3; // 5 bits
            //g = (p_pixel[1] & 0xFC) >> 2; // 6 bits
            g = (p_pixel[1] & 0xF8) >> 3; // 5 bits
            r = (p_pixel[2] & 0xF8) >> 3; // 5 bits
            p = (r << 10) | (g << 5) | (b << 0);
            G_memory[m++] = p & 0xFF;
            G_memory[m++] = (p >> 8) & 0xFF;
        }
    }
}

//NXP Based
void render5650(void)
{
    char *p_pixel = G_imageTGA+18;
    unsigned int p, r, g, b;
    unsigned int m=0;
    int x, y;
    for (y=0; y<G_VideoHeight; y++) {
        p_pixel = G_imageTGA + 18 + ((G_VideoHeight-1-y) * G_VideoWidth * 3);
        for (x=0; x<G_VideoWidth; x++, p_pixel+=3) {
            b = (p_pixel[0] & 0xF8) >> 3; // 5 bits
            g = (p_pixel[1] & 0xFC) >> 2; // 6 bits
			//g = (p_pixel[1] & 0xF8) >> 3; // 5 bits
            r = (p_pixel[2] & 0xF8) >> 3; // 5 bits
            p = (r<<10)|(g<<5)|(b<<0);
            G_memory[m++] = p & 0xFF;
            G_memory[m++] = (p>>8) & 0xFF;
        }
    }
}

//Synergy Bases
void render5651(void)
{
    char *p_pixel = G_imageTGA+18;
    unsigned int p, r, g, b;
    unsigned int m=0;
    int x, y;
    for (y=0; y<G_VideoHeight; y++) {
        p_pixel = G_imageTGA + 18 + ((G_VideoHeight-1-y) * G_VideoWidth * 3);
        for (x=0; x<G_VideoWidth; x++, p_pixel+=3) {
            b = (p_pixel[0] & 0xF8) >> 3; // 5 bits
            g = (p_pixel[1] & 0xFC) >> 2; // 6 bits
			//g = (p_pixel[1] & 0xF8) >> 3; // 5 bits
            r = (p_pixel[2] & 0xF8) >> 3; // 5 bits
            p = (r<<11)|(g<<5)|(b<<0);
            G_memory[m++] = p & 0xFF;
            G_memory[m++] = (p>>8) & 0xFF;
        }
    }
}

void renderGray(void)
{
    char *p_pixel = G_imageTGA+18;
    unsigned int p, r, g, b;
    unsigned int m=0;
    int x, y;
    for (y=0; y<G_VideoHeight; y++) {
        p_pixel = G_imageTGA + 18 + ((G_VideoHeight-1-y) * G_VideoWidth * 1);
        for (x=0; x<G_VideoWidth; x++, p_pixel+=1) {
            b = (p_pixel[0] & 0xF8) >> 3; // 5 bits
            g = (p_pixel[0] & 0xFC) >> 2; // 6 bits
            r = (p_pixel[0] & 0xF8) >> 3; // 5 bits
            p = (r<<11)|(g<<5)|(b<<0);
            G_memory[m++] = p & 0xFF;
            G_memory[m++] = (p>>8) & 0xFF;
        }
    }
}

void uncompressTGA(int length)
{
    int num = G_VideoWidth*G_VideoHeight;
    int i;
    unsigned char n;
    unsigned char *p;
    unsigned char *q;
    memcpy(G_uncompressedTGA, G_imageTGA, 18);
    q = (unsigned char *)(G_uncompressedTGA+18);
    length -= 18;
    p = (unsigned char *)(G_imageTGA+18);
    while ((length>4) && (num>0)) {
        n = *(p++);
        length--;
        if (n & 0x80) {
            n &= 0x7F;
            n++;
            for (i=0; (i<n) && (num>0); i++) {
                memcpy(q, p, 3);
                q += 3;
                num--;
            }
            length-=3;
            p+=3;
        } else {
            n++;
            if (n > num)
                n = num;
            memcpy(q, p, 3*n);
            length -= 3*n;
            p+=3*n;
            num -= n;
        }
    }
    memcpy(G_imageTGA, G_uncompressedTGA, 18+3*G_VideoWidth*G_VideoHeight);
}

int _tmain(int argc, _TCHAR* argv[])
{
    int start, end, color;
    int length;
    char filename[1000];

    printf("VideoTGAtoBIN v1.01\n");
    if (argc != 8) {
        printf("VideoTGAtoBIN <color format> < \"VIDE%%04d.TGA\"> <output file> <start frame> <end frame> <video width> <video height>\n");
        printf("<color format> is 5550, 5650 or 5651\n");
        printf("5650 is for NXP, 5651 for Synergy\n");
        printf("5550 is for uEZGUIs on current uEZ version, 5650 may have been supported in some uEZ versions or could be supported as alternate option.\n");
        printf("Possibly up to 1920x1080 frame size is supported, but not tested. Increase array sizes for larger frame size and working memory as needed.\n");
        exit(1);
    }
    color = atoi(argv[1]); // color format argument
	// input file is argv[2] used BELOW in the for loop.
    FILE *fout = fopen(argv[3], "wb"); // output file argument.
    if (!fout) {
        printf("Could not open output file %s\n", argv[3]);
        exit(2);
    }
    
    // last 4 arguments are self explanitory.
    start = atoi(argv[4]);
    end = atoi(argv[5]);
	G_VideoWidth = atoi(argv[6]);
	G_VideoHeight = atoi(argv[7]);
    for (int frame=start; frame<=end; frame++) {
        printf("Frame %d ... ", frame);
        sprintf(filename, argv[2], frame);
        FILE *fp = fopen(filename, "rb");
        if (!fp)  {
            printf("Could not open input file %s\n", argv[2]);
            exit(3);
        }
        length = filelength(fileno(fp));
        fread(G_imageTGA, 1, sizeof(G_imageTGA), fp);
        fclose(fp);
        if (G_imageTGA[2] == 10) {
            printf("Uncompressing ... ");
            uncompressTGA(length);
        } 
        if (G_imageTGA[2] == 3) {
            printf("Grayscale ... ");
            renderGray();
        } else {
            if (color == 5550) {
                render5550();
            }
            else if (color == 5650) {
                render5650();
            }
            else if (color == 5651) {
                render5651();
            }
            else {
                printf("Color format not supported\n");
            }
        }
        fwrite(G_memory, 1, G_VideoWidth*G_VideoHeight*2, fout);
        printf("OK\n");
    }
    fclose(fout);
    printf("Done.\n");

	return 0;
}

