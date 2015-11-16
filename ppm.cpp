#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ppm.h"

Ppmimage *ppm6GetImage(const char *filename)
{
	int i, j, width, height, size, maxval, ntries;
	char ts[4096];
	//unsigned int rgb[3];
	unsigned char c;
	unsigned char *p;
	FILE *fpi;
	Ppmimage *image = (Ppmimage *)malloc(sizeof(Ppmimage));
	if (!image) {
		printf("ERROR: out of memory\n");
		exit(EXIT_FAILURE);
	}
	fpi = fopen(filename, "r");
	if (!fpi) {
		printf("ERROR: cannot open file **%s** for reading.\n", filename);
		exit(EXIT_FAILURE);
	}
	image->data=NULL;
	char *x = fgets(ts, 4, fpi);
	if (x){}
	if (strncmp(ts, "P6" ,2) != 0) {
		printf("ERROR: File is not ppm RAW format.\n");
		exit(EXIT_FAILURE);
	}
	//comments?
	while(1) {
		c = fgetc(fpi);
		if (c != '#')
			break;
		//read until newline
		ntries=0;
		while(1) {
			//to avoid infinite loop...
			if (++ntries > 10000) {
				printf("ERROR: too many blank lines in **%s**\n", filename);
				exit(EXIT_FAILURE);
			}
			c = fgetc(fpi);
			if (c == '\n')
				break;
		}
	}
	ungetc(c, fpi);
	int r = fscanf(fpi, "%u%u%u", &width, &height, &maxval);
	if (r){}
	//
	//get past any newline or carrage-return
	ntries=0;
	while(1) {
		//to avoid infinite loop...
		if (++ntries > 10000) {
			printf("ERROR: too many blank lines in **%s**\n", filename);
			exit(EXIT_FAILURE);
		}
		c = fgetc(fpi);
		if (c != 10 && c != 13) {
			ungetc(c, fpi);
			break;
		}
	}
	//
	size = width * height * 3;
	image->data = (unsigned char *)malloc(size);
	if (!image->data) {
		printf("ERROR: no memory for image data.\n");
		exit(EXIT_FAILURE);
	}
	image->width = width;
	image->height = height;
	p = (unsigned char *)image->data;
	for (i=0; i<height; i++) {
		for (j=0; j<width; j++) {
			*p = fgetc(fpi); p++;
			*p = fgetc(fpi); p++;
			*p = fgetc(fpi); p++;
		}
	}
	fclose(fpi);
	return image;
}

void ppm6CleanupImage(Ppmimage *image)
{
	if (image != NULL) {
		if (image->data != NULL)
			free(image->data);
		free(image);
	}
}

Ppmimage *ppm6CreateImage(int width, int height)
{
	//printf("ppm6_create_image()...\n");
	int size = width * height * 3;
	Ppmimage *image = (Ppmimage *)malloc(sizeof(Ppmimage));
	if (!image) {
		printf("ERROR: no memory for image.\n");
		exit(EXIT_FAILURE);
	}
	image->data = (unsigned char *)malloc(size);
	if (!image->data) {
		printf("ERROR: no memory for image data.\n");
		exit(EXIT_FAILURE);
	}
	image->width = width;
	image->height = height;
	return image;
}

void ppm6SaveImage(Ppmimage *image, const char *filename)
{
	int i,j;
	//int size = image->width * image->height * 3;
	unsigned char *p = (unsigned char *)image->data;
	FILE *fpo = fopen(filename, "w");
	if (!fpo) {
		printf("ERROR: cannot open file **%s** for writing.\n", filename);
		exit(EXIT_FAILURE);
	}
	fprintf(fpo, "P6\n%d %d\n%d\n", image->width, image->height, 255);
	for (i=0; i<image->height; i++) {
		for (j=0; j<image->width; j++) {
			fputc(*p, fpo); p++;
			fputc(*p, fpo); p++;
			fputc(*p, fpo); p++;
		}
	}
	fclose(fpo);
}

void ppm6ClearImage(Ppmimage *image, unsigned char red, unsigned char green, unsigned char blue)
{
	int i;
	int pix = image->width * image->height;
	unsigned char *data = (unsigned char *)image->data;
	for (i=0; i<pix; i++) {
		*data = red; data++;
		*data = green; data++;
		*data = blue; data++;
	}
}

void ppm6Setpixel(Ppmimage *image, int x, int y, int channel, unsigned char val) {
	unsigned char *data = (unsigned char *)image->data + ((y * image->width * 3) + (x * 3) + channel);
	*data = val;
}
