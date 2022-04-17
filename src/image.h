#ifndef IMAGE_H
#define IMAGE_H

#include "util.h"
#include <png.h>

struct Image
{
    int w, h;
    png_bytep *rows;

    png_structp ptr;
    png_infop info;
};

struct Image *image_alloc(const char *src);
void image_free(struct Image *img);

Vec3f image_at(struct Image *img, int x, int y);

#endif

