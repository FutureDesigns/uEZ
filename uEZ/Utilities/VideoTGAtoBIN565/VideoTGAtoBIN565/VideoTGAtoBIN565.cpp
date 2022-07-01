// VideoTGAtoBIN565.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <io.h>

char G_imageTGA[1000000];
char G_uncompressedTGA[1000000];

unsigned char G_memory[1000000];

int G_VideoHeight;
int G_VideoWidth;

void render(void)
{
    char *p_pixel = G_imageTGA+18;
    unsigned int p, r, g, b;
    unsigned int m=0;
    int x, y;
    for (y=0; y<G_VideoHeight; y++) {
        p_pixel = G_imageTGA + 18 + ((G_VideoHeight-1-y) * G_VideoWidth * 3);
        for (x=0; x<G_VideoWidth; x++, p_pixel+=3) {
            b = (p_pixel[0] & 0xF8) >> 3; // 5 bits
            //g = (p_pixel[1] & 0xFC) >> 2; // 6 bits
			g = (p_pixel[1] & 0xF8) >> 3; // 5 bits
            r = (p_pixel[2] & 0xF8) >> 3; // 5 bits
            p = (r<<10)|(g<<5)|(b<<0);
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
    int start, end;
    int length;
    char filename[1000];

    printf("VideoTGAtoBIN565 v1.00\n");
    if (argc != 7) {
        printf("VideoTGAtoBIN565 <file format \"COKE%04d.TGA\"> <output file> <start frame> <end frame> <video width> <video height>\n");
        exit(1);
    }
    FILE *fout = fopen(argv[2], "wb");
    if (!fout) {
        printf("Could not open output file %s\n", argv[2]);
        exit(2);
    }
    start = atoi(argv[3]);
    end = atoi(argv[4]);
	G_VideoWidth = atoi(argv[5]);
	G_VideoHeight = atoi(argv[6]);
    for (int frame=start; frame<=end; frame++) {
        printf("Frame %d ... ", frame);
        sprintf(filename, argv[1], frame);
        FILE *fp = fopen(filename, "rb");
        if (!fp)  {
            printf("Could not open input file %s\n", argv[1]);
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
            render();
        }
        fwrite(G_memory, 1, G_VideoWidth*G_VideoHeight*2, fout);
        printf("OK\n");
    }
    fclose(fout);
    printf("Done.\n");

	return 0;
}

