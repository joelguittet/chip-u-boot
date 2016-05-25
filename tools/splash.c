/*
 * Copyright (C) 2016 Next Thing Co.
 * Jose Angel Torres <software@nextthing.co>
 * 
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include "splash.h"
#include "compiler.h"
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

void skip_bytes (FILE *fp, int n)
{
        while (n-- > 0)
                fgetc (fp);
}


int main (int argc, char *argv[])
{
	//int	i, x;
	FILE	*fp;
	bitmap_t bmp;
	bitmap_t *b = &bmp;
	uint16_t data_offset, n_colors;
	DIR *dp;
	struct dirent *ep;
	
	printf("%s",argv[1]);
	dp = opendir (argv[1]);
	if (dp != NULL)
	{
		while (ep = readdir (dp))
			puts (ep->d_name);
		(void) closedir (dp);
	 }
	 else 
	{
		printf ("Couldn't open the directory\n");
		exit (EXIT_FAILURE);
	}
	//parse bitmaps in splash directory
	
	fp = fopen("./images/1.bmp" , "rb");
	if (!fp) {
		perror(argv[2]);
		exit (EXIT_FAILURE);
	}

	if (fgetc (fp) != 'B' || fgetc (fp) != 'M') {
		printf("Input file is not a bitmap\n");
		exit(EXIT_FAILURE);
	}

	/*
	 * read width and height of the image, and the number of colors used;
	 * ignore the rest
	 */
	skip_bytes (fp, 8);
	if (fread (&data_offset, sizeof (uint16_t), 1, fp) != 1)
		printf("Couldn't read bitmap data offset\n");
	skip_bytes (fp, 6);
	if (fread (&b->width,   sizeof (uint16_t), 1, fp) != 1)
		printf("Couldn't read bitmap width\n");
	skip_bytes (fp, 2);
	if (fread (&b->height,  sizeof (uint16_t), 1, fp) != 1)
		printf("Couldn't read bitmap height\n");
	skip_bytes (fp, 22);
	if (fread (&n_colors, sizeof (uint16_t), 1, fp) != 1)
		printf("Couldn't read bitmap colors\n");
	skip_bytes (fp, 6);

}
