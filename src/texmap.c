#include "texmap.h"


struct Texmap *tex_alloc(Vec3f pos, int w, int h, const char *src)
{
    struct Texmap *tex = malloc(sizeof(struct Texmap));
    tex->pos = pos;
    tex->w = w;
    tex->h = h;
    tex->img = image_alloc(src);

    return tex;
}


void tex_free(struct Texmap *tex)
{
    image_free(tex->img);
    free(tex);
}


Vec3f tex_color(struct Texmap *tex, Vec3f orig, Vec3f dir, Vec3f default_col)
{
    // find intersection point
    Vec3f norm = { 0.f, 0.f, -1.f };
    float t = (vec_dot(tex->pos, norm) - vec_dot(orig, norm)) / vec_dot(dir, norm);

    Vec3f hit = vec_addv(orig, vec_mulf(dir, t));
    
    float x = hit.x - tex->pos.x;
    float y = hit.y - tex->pos.y;

    int ix = (x / (float)tex->w) * (float)tex->img->w;
    int iy = (y / (float)tex->h) * (float)tex->img->h;

    if (ix >= 0 && iy >= 0 && ix < tex->img->w && iy < tex->img->h)
        return image_at(tex->img, ix, iy);
    else
        return default_col;
}

