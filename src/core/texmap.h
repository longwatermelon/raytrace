#ifndef CORE_TEXMAP_H
#define CORE_TEXMAP_H

#include "image.h"

struct Texmap
{
    Vec3f pos;
    int w, h;

    struct Image *img;
};

struct Texmap *tex_alloc(Vec3f pos, int w, int h, const char *src);
void tex_free(struct Texmap *tex);

Vec3f tex_color(struct Texmap *tex, Vec3f orig, Vec3f dir, Vec3f default_col);

#endif

