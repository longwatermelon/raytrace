#include "light.h"


struct Light *light_alloc(Vec3f pos, float in)
{
    struct Light *l = malloc(sizeof(struct Light));
    l->pos = pos;
    l->in = in;

    return l;
}


void light_free(struct Light *l)
{
    free(l);
}


struct Material *mat_alloc(Vec3f col, float specular_exp, float rd, float rs, float rm)
{
    struct Material *m = malloc(sizeof(struct Material));
    m->col = col;
    m->specular_exp = specular_exp;

    m->ref_diffuse = rd;
    m->ref_specular = rs;
    m->ref_mirror = rm;

    return m;
}


void mat_free(struct Material *m)
{
    free(m);
}

