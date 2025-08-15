//doomgeneric for a bare Linux VirtualTerminal
// Copyright (C) 2025 Techflash
// Based on doomgeneric_sdl.c

#include "doomkeys.h"
#include "m_argv.h"
#include "doomgeneric.h"
#include "i_system.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/time.h>


long frames = 0;
static struct timeval startTime;

uint8_t *BMP = NULL;
unsigned char * BASE64 = NULL;

#define PALETTE_SIZE 256

#define BMP_HEADER_SIZE 54

uint8_t * init_bmp(uint8_t *rgba, int width, int height, size_t *bmp_size) {
    int row_stride = (width * 3 + 3) & ~3; // pad to 4 bytes
    int pixel_data_size = row_stride * height;
    *bmp_size = (size_t)(BMP_HEADER_SIZE + pixel_data_size);

    memset(BMP, 0, BMP_HEADER_SIZE);
    BMP[0] = 'B';
    BMP[1] = 'M';
    *(uint32_t*)&BMP[2]  = (uint32_t)(*bmp_size);
    *(uint32_t*)&BMP[10] = BMP_HEADER_SIZE;
    *(uint32_t*)&BMP[14] = 40; // DIB header size
    *(int32_t*)&BMP[18]  = width;
    *(int32_t*)&BMP[22]  = height;
    *(uint16_t*)&BMP[26] = 1;   // planes
    *(uint16_t*)&BMP[28] = 24;  // bits per pixel
    *(uint32_t*)&BMP[34] = pixel_data_size;
    return BMP;
}


void DG_Init(void) {
    printf("\033[2J");  // clear screen
    printf("\033[H");  // home cursor
    printf("\033[?25l"); // hide cursor
    if (!BMP) {
        int maxsizes = (4 * (( 2*DOOMGENERIC_RESX * 2*DOOMGENERIC_RESY +  PALETTE_SIZE*3 + 2) / 3)+1) ;
        BMP = (uint8_t*)malloc(maxsizes);
        BASE64 = (unsigned char*)malloc(maxsizes);
        if (!BMP || !BASE64) {
            perror("malloc");
            return;
        }
        // fill the bmp header once
        size_t bmp_size;
        init_bmp((unsigned char*)DG_ScreenBuffer, DOOMGENERIC_RESX, DOOMGENERIC_RESY, &bmp_size);

    }
	// get the start time
	gettimeofday(&startTime, NULL);
}


static const char base64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

base64_encode(const uint8_t *data, size_t input_length) {
    size_t output_length = 4 * ((input_length + 2) / 3);

    for (size_t i = 0, j = 0; i < input_length;) {
        uint32_t octet_a = i < input_length ? data[i++] : 0;
        uint32_t octet_b = i < input_length ? data[i++] : 0;
        uint32_t octet_c = i < input_length ? data[i++] : 0;

        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        BASE64[j++] = base64_table[(triple >> 18) & 0x3F];
        BASE64[j++] = base64_table[(triple >> 12) & 0x3F];
        BASE64[j++] = (i > input_length + 1) ? '=' : base64_table[(triple >> 6) & 0x3F];
        BASE64[j++] = (i > input_length) ? '=' : base64_table[triple & 0x3F];
    }
    BASE64[output_length] = '\0';
}



void store_bmp(uint8_t *rgba, int width, int height, size_t *bmp_size) {
    int row_stride = (width * 3 + 3) & ~3; // pad to 4 bytes
    int pixel_data_size = row_stride * height;
    *bmp_size = (size_t)(BMP_HEADER_SIZE + pixel_data_size);

    // BMP Header already set, see init_bmp()

    // TODO: pre encode  the header to b64, remove end mark and stream b64 directly from here

    // --- Pixel Data ---
    uint8_t *pixels = BMP + BMP_HEADER_SIZE;
    for (int y = 0; y < height; y++) {
        uint8_t *row = pixels + (height - 1 - y) * row_stride;
        for (int x = 0; x < width; x++) {
            int i = (y * width + x) * 4;
            memcpy(&row[x * 3], &rgba[i], 3);
        }
        // Padding
        for (int p = width * 3; p < row_stride; p++) {
            row[p] = 0;
        }
    }
}


void DG_DrawFrame() {
	if (!DG_ScreenBuffer) {
		return;
	}
    size_t bmp_size;
    store_bmp((unsigned char*)DG_ScreenBuffer, DOOMGENERIC_RESX, DOOMGENERIC_RESY, &bmp_size);
    base64_encode(BMP, bmp_size);
    printf("\033[H;\033]1337;File=inline=1;width=auto;height=auto;preserveAspectRatio=1;name=bmpdump;size=%zu:%s\a\n", bmp_size, BASE64);
    puts("frame");
/*
	checkKeys();
*/
}

void DG_SleepMs(uint32_t ms) {
	usleep(ms * 1000);
}

uint32_t DG_GetTicksMs(void) {
	struct timeval curTime;
	long seconds, usec;

	gettimeofday(&curTime, NULL);
	seconds = curTime.tv_sec - startTime.tv_sec;
	usec = curTime.tv_usec - startTime.tv_usec;

	return (seconds * 1000) + (usec / 1000);
}

int DG_GetKey(int* pressed, unsigned char* doomKey) {
/*
	checkKeys();

	if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex) {
		//key queue is empty

		return 0;
	}
	else {
		unsigned short keyData = s_KeyQueue[s_KeyQueueReadIndex];
		s_KeyQueueReadIndex++;
		s_KeyQueueReadIndex %= KEYQUEUE_SIZE;

		*pressed = keyData >> 8;
		*doomKey = keyData & 0xFF;

		return 1;
	}
*/
    return 0;
}

void DG_SetWindowTitle(const char * title) {
	printf("Window Title: %s\r\n", title);
}

int main(int argc, char **argv) {
	doomgeneric_Create(0, NULL);
	while (1)
	{
		doomgeneric_Tick();
	}

	return 0;
}
