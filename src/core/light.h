#ifndef LIGHT_H
#define LIGHT_H

#include "util.h"
#include "texmap.h"

struct Light
{
    Vec3f pos;
    float in; // intensity
};

struct Light *light_alloc(Vec3f pos, float in);
void light_free(struct Light *l);

struct Material
{
    Vec3f col;
    float specular_exp;

    float ref_diffuse, ref_specular, ref_mirror; // reflectiveness of diffuse, specular, and mirror (0.f - 1.f)
    
    struct Texmap *tex;
};

struct Material *mat_alloc(Vec3f col, float specular_exp, float rd, float rs, float rm, struct Texmap *tex);
void mat_free(struct Material *m);

#endif

