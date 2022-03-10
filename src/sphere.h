#ifndef SPHERE_H
#define SPHERE_H

#include "vector.h"
#include <stdbool.h>
#include <SDL2/SDL.h>

struct Sphere
{
    Vec3f c; // center
    float r; // radius

    Vec3f col;
};

struct Sphere *sphere_alloc(Vec3f c, float r, Vec3f col);
void sphere_free(struct Sphere *s);

// ro: ray origin
// t: nearest intersection
bool sphere_ray_intersect(struct Sphere *s, Vec3f ro, Vec3f rdir, float *t);

#endif
