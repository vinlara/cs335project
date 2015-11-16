#ifndef _PPM_H_
#define _PPM_H_

typedef struct t_ppmimage {
	int width;
	int height;
	void *data;
} Ppmimage;

extern Ppmimage *ppm6CreateImage(int width, int height);
extern Ppmimage *ppm6GetImage(const char *filename);
extern void ppm6ClearImage(Ppmimage *image, unsigned char r, unsigned char g, unsigned char b);
extern void ppm6SaveImage(Ppmimage *image, const char *filename);
extern void ppm6Setpixel(Ppmimage *image, int x, int y, int channel, unsigned char val);
extern void ppm6CleanupImage(Ppmimage *image);

#endif
