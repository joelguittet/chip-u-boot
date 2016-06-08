/*
 * Copyright (C) 2016 Next Thing Co.
 * Jose Angel Torres <software@nextthing.co>
 * 
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include "splash_tool.h"
#include "compiler.h"
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

/*
 * Neutralize little endians.
 */
uint16_t le_short(uint16_t x)
{
    uint16_t val;
    uint8_t *p = (uint8_t *)(&x);

    val =  (*p++ & 0xff) << 0;
    val |= (*p & 0xff) << 8;

    return val;
}

void skip_bytes (FILE *fp, int n)
{
        while (n-- > 0)
                fgetc (fp);
}

int ends_with(const char* name, const char* extension, size_t length)
{
  const char* ldot = strrchr(name, '.');
  if (ldot != NULL)
  {
    if (length == 0)
      length = strlen(extension);
    return strncmp(ldot + 1, extension, length) == 0;
  }
  return 0;
}

char *remove_ext (char* mystr, char dot, char sep) {
    char *retstr, *lastdot, *lastsep;

    /* Error checks and allocate string. */
    if (mystr == NULL)
        return NULL;
    if ((retstr = malloc (strlen (mystr) + 1)) == NULL)
        return NULL;

    /* Make a copy and find the relevant characters. */
    strcpy (retstr, mystr);
    lastdot = strrchr (retstr, dot);
    lastsep = (sep == 0) ? NULL : strrchr (retstr, sep);

    /* If it has an extension separator. */
    if (lastdot != NULL) {
        /* and it's before the extenstion separator. */
        if (lastsep != NULL) {
            if (lastsep < lastdot) {
                /* then remove it. */
                *lastdot = '\0';
            }
        } else {
            /* Has extension separator with no path separator. */
            *lastdot = '\0';
        }
    }

    /* Return the modified string. */

    return retstr;
}

int is_bmp(const char *name)
{
        return ends_with(name, "bmp", 3);
}

void usage(const char *prog)
{
	printf("Usage: %s [--gen-info|--gen-data] folder\n", prog);
}

int main (int argc, char *argv[])
{
	char FilenameArr[20][256];
	FILE	*fp;
	bitmap_t bmp;
	bitmap_t *b = &bmp;
	DIR *dp;
	struct dirent *ep;
	int i = 0, k;
	char filename [256];
	int mode, cx;
	char bmp_name[256];
	
	if (!strcmp(argv[1], "--gen-info"))
		mode = MODE_GEN_INFO;
	else if (!strcmp(argv[1], "--gen-data"))
		mode = MODE_GEN_DATA;
	else {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	
	dp = opendir (argv[2]);
	if (dp != NULL)
	{
		while ((ep = readdir (dp))) {
			if (is_bmp(ep->d_name)){
				strcpy(FilenameArr[i],ep->d_name); 
				if (i < 20)
					i++;
				else {
					fprintf(stderr, "Exceeded limit of 20 bitmaps!");
					exit(EXIT_FAILURE);
				}
			}
		}	
		(void) closedir (dp);
	}
	else 
    {
		 fprintf(stderr, "%s\n", "Couldn't open the directory");
		 exit (EXIT_FAILURE);
	}
	
	print_header(mode);
	
    //print the filenames on the screen	
	for (k=0;(k<i) && (k<20);k++) {
		fprintf(stderr, "Processing: %s\n", FilenameArr[k]);
	
		cx = snprintf ( filename, 256, "%s/%s", argv[2], FilenameArr[k] );
		if (!(cx>=0 && cx<256)) {
			fprintf(stderr, "Filename error!");
			exit(EXIT_FAILURE);
		}
		
		fp = fopen(filename , "rb");
		if (!fp) {
			perror(filename);
			exit (EXIT_FAILURE);
		}
		
		cx = snprintf ( bmp_name, 256, "bitmap_%d", k);
		if (!(cx>=0 && cx<256)) {
			fprintf(stderr, "Filename error!");
			exit(EXIT_FAILURE);
		}
		
		get_bitmap_info(b, fp);
		
		if (mode == MODE_GEN_DATA)
		{
			//Generate Bitmap Data
			print_bitmap_data(b, fp, bmp_name);
		}
		fclose(fp);
	}

	print_footer(mode, k);	

	return 0;
}

void get_bitmap_info(bitmap_t *b, FILE *fp)
{

	//Check if bitmap file
		if (fgetc (fp) != 'B' || fgetc (fp) != 'M') {
                fprintf(stderr, "%s\n", "Input file is not a bitmap");
                exit(EXIT_FAILURE);
        }

        /*
         * read width and height of the image, and the number of colors used;
         * ignore the rest
         */
        skip_bytes(fp, 8);
        if (fread(&b->data_offset, sizeof (uint16_t), 1, fp) != 1)
                fprintf(stderr, "%s\n", "Couldn't read bitmap data offset\n");
        /* width */
        skip_bytes(fp, 6);
        if (fread(&b->width,   sizeof (uint16_t), 1, fp) != 1)
                fprintf(stderr, "%s\n", "Couldn't read bitmap width\n");
        /* height */
        skip_bytes(fp, 2);
        if (fread(&b->height,  sizeof (uint16_t), 1, fp) != 1)
                fprintf(stderr, "%s\n", "Couldn't read bitmap height\n");
        /* bit_count */
        skip_bytes(fp, 4);
        if (fread(&b->bit_count,  sizeof (uint16_t), 1, fp) != 1)
                fprintf(stderr, "%s\n", "Couldn't read bitmap bit_count\n");
        /* compression */
        if (fread(&b->compression,  sizeof (uint16_t), 1, fp) != 1)
                fprintf(stderr, "%s\n", "Couldn't read bitmap compression\n");
        /* colors */
        skip_bytes(fp, 14);
        if (fread(&b->n_colors, sizeof (uint16_t), 1, fp) != 1)
                fprintf(stderr, "%s\n", "Couldn't read bitmap colors\n");
        skip_bytes(fp, 6);

        /*
         * Repair endianess.
         */
        b->data_offset = le_short(b->data_offset);
        b->width = le_short(b->width);
        b->height = le_short(b->height);
        b->n_colors = le_short(b->n_colors);

        /* assume we are working with an 8-bit file */
        if ((b->n_colors == 0) || (b->n_colors > 256 - DEFAULT_CMAP_SIZE)) {
                /* reserve DEFAULT_CMAP_SIZE color map entries for default map */
        		b->n_colors = 256 - DEFAULT_CMAP_SIZE;
        }
}  


void print_header(int mode) {
	printf("/*\n"
           " * Automatically generated by \"tools/splash\"\n"
           " *\n"
           " * DO NOT EDIT\n"
           " *\n"
           " */\n\n\n" );
	if(mode == MODE_GEN_INFO)
	{
		printf("#ifndef __SPLASH_INFO_H__\n"
				"#define __SPLASH_INFO_H__\n\n");
		
		printf("typedef struct bmp_properties {\n"
				"	int width;\n"
				"	int height;\n"
				"	int colors;\n"
				"	int offset;\n"
				"	int bit_count;\n"
				"	int compression;\n"
				"} bmp_properties;\n\n");
	}
	else if (mode == MODE_GEN_DATA)
	{
		printf("#include <splash_info.h>\n\n");
	}
	else
    {
		 fprintf(stderr, "Incorrect Mode Setting!\n");
		 exit (EXIT_FAILURE);
	}
}

void print_footer(int mode, int num_splashes) {
	int i;
	
	if(mode == MODE_GEN_INFO)
	{
		printf("extern bmp_properties * properties[];\n"
				"extern unsigned short * palettes[];\n"
				"extern unsigned char * bitmaps[];\n\n");
		
		printf("#define NUM_SPLASHES  %d\n\n", num_splashes);
		
		printf("#endif /* __SPLASH_INFO_H__ */\n\n");
	}
	else if (mode == MODE_GEN_DATA)
	{
		printf("bmp_properties * properties[NUM_SPLASHES] = {");
		for (i = 0; i < num_splashes; i++)
		{
			printf(" &bitmap_%d_prop,", i);
		}
				
		printf( " };\n\n");		
		
		printf(	"unsigned short * palettes[NUM_SPLASHES] = {");
		for (i = 0; i < num_splashes; i++)
		{
			printf(" bitmap_%d_palette,", i);
		}
		
		printf(" };\n");
		
		printf("unsigned char * bitmaps[NUM_SPLASHES] = {");
		for (i = 0; i < num_splashes; i++)
		{
			printf(" bitmap_%d,",  i);
		}
		
		printf(" };\n\n");
	}
	else
    {
		 fprintf(stderr, "Incorrect Mode Setting!\n");
		 exit (EXIT_FAILURE);
	}        
}

void print_bitmap_data(bitmap_t *b, FILE *fp, char *bmp_name) {
	int i, x;
    /* allocate memory */
    if ((b->data = (uint8_t *)malloc(b->width * b->height)) == NULL)
    	perror("Error allocating memory for file");

	//TODO: Get filename and replace below in platette variable

        /* read and print the palette information */
        printf("unsigned short %s_palette[] = {\n", bmp_name);

        for (i=0; i<b->n_colors; ++i) {
                b->palette[(int)(i*3+2)] = fgetc(fp);
                b->palette[(int)(i*3+1)] = fgetc(fp);
                b->palette[(int)(i*3+0)] = fgetc(fp);
                x=fgetc(fp);

                printf("%s0x0%X%X%X,%s",
                        ((i%8) == 0) ? "\t" : "  ",
                        (b->palette[(int)(i*3+0)] >> 4) & 0x0F,
                        (b->palette[(int)(i*3+1)] >> 4) & 0x0F,
                        (b->palette[(int)(i*3+2)] >> 4) & 0x0F,
                        ((i%8) == 7) ? "\n" : ""
                );
        }

    /* seek to offset indicated by file header */
    fseek(fp, (long)b->data_offset, SEEK_SET);

    /* read the bitmap; leave room for default color map */
    printf("\n");
    printf("};\n");
    printf("\n");

    printf("unsigned char %s[] = {\n", bmp_name);
        for (i = (b->height-1)*b->width; i >= 0; i -= b->width) {
                for (x = 0; x < b->width; x++) {
                        b->data[i + x] = (uint8_t) fgetc(fp)
                                                + DEFAULT_CMAP_SIZE;
                }
        }

        for (i = 0; i < (b->height*b->width); ++i) {
                if ((i%8) == 0)
                        putchar ('\t');
                printf ("0x%02X,%c",
                        b->data[i],
                        ((i%8) == 7) ? '\n' : ' '
                );
        }

    printf ("\n"
            "};\n\n"
    );
 
	printf("bmp_properties %s_prop = {"
		" .width = %d, .height = %d,"
		" .colors = %d, .offset = %d,"
		" .bit_count = %d, .compression = %d };\n\n",
		bmp_name, b->width, b->height, b->n_colors, DEFAULT_CMAP_SIZE, 
		b->bit_count, b->compression);

}

