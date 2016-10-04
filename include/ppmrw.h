#ifndef PPMRW_H
#define PPMRW_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE 0
#define TRUE 1
#define MAX_SIZE 1024

typedef int8_t boolean;

typedef struct header_t {
    int file_type;
    char **comments;
    int width;
    int height;
    int max_color_val;
} header;

typedef struct RGBPixel_t {
    unsigned char r, g, b;
} RGBPixel;

typedef struct image_t {
    RGBPixel *map;
    int width, height, max_color_val;
} image;

void print_pixels(RGBPixel *map, int width, int height);
void ppm_create(FILE *fh, int type, image *img);
#endif