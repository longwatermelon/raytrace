#include "camera.h"


struct Camera *cam_alloc(Vec3f pos, Vec3f angle)
{
    struct Camera *c = malloc(sizeof(struct Camera));
    c->pos = pos;
    c->angle = angle;

    return c;
}


void cam_free(struct Camera *c)
{
    free(c);
}

