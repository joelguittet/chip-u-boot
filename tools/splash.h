/*
 * Copyright (C) 2016 Next Thing Co.
 * Jose Angel Torres <software@nextthing.co>
 * 
 * SPDX-License-Identifier:	GPL-2.0+
 */


#ifndef __SPLASH_H__
#define __SPLASH_H__

enum {
	MODE_GEN_INFO,
	MODE_GEN_DATA
};

typedef struct bitmap_s {		/* bitmap description */
	uint16_t width;
	uint16_t height;
	uint8_t	palette[256*3];
	uint8_t	*data;
} bitmap_t;

#define DEFAULT_CMAP_SIZE	16	/* size of default color map	*/



#endif /*__SPLASH_H__ */