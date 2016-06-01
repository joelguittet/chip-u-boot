/*
 * Copyright (C) 2016 Next Thing Co.
 * Jose Angel Torres <software@nextthing.co>
 * 
 * SPDX-License-Identifier:	GPL-2.0+
 */


#ifndef __SPLASH_TOOL_H__
#define __SPLASH_TOOL_H__

enum {
	MODE_GEN_INFO,
	MODE_GEN_DATA
};

typedef struct bitmap_s {		/* bitmap description */
	uint16_t width;
	uint16_t height;
	uint16_t bit_count;
	uint16_t data_offset;
	uint16_t compression;
	uint16_t n_colors;
	uint8_t	palette[256*3];
	uint8_t	*data;
} bitmap_t;

#define DEFAULT_CMAP_SIZE	16	/* size of default color map	*/

void get_bitmap_info(bitmap_t *b, FILE *fp);

void print_header(int mode);
void print_footer(int mode, int num_splashes);
void print_bitmap_data(bitmap_t *b, FILE *fp, char * bmp_name);
void print_bitmap_info(bitmap_t *b, char * bmp_name, int splash_num);

#endif /*__SPLASH_TOOL_H__ */