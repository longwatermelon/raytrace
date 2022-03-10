#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include <stdbool.h>
#include <sys/types.h>
#include <SDL2/SDL.h>

typedef struct
{
    int idx[3];
    Vec3f norm;
} Triangle;

struct Mesh
{
    Vec3f pos;

    Vec3f *pts;
    size_t npts;

    Triangle *tris;
    size_t ntris;

    Vec3f col;

    bool invert_normal;

    // optimization
    float top_ry, bot_ry; // ray y component for highest and lowest point
};

struct Mesh *mesh_alloc(Vec3f pos, const char *fp, Vec3f col);
void mesh_free(struct Mesh *m);

void mesh_read(struct Mesh *m, const char *fp);

// ro: ray origin
// t: nearest intersection
bool mesh_ray_intersect(struct Mesh *m, Vec3f ro, Vec3f rdir, float *t, Triangle *tri);
bool mesh_ray_tri_intersect(struct Mesh *m, Triangle tri, Vec3f ro, Vec3f rdir, float *t);

void mesh_find_bounds(struct Mesh *m, Vec3f ro);

Vec3f mesh_tri_normal(struct Mesh *m, Triangle t);

#endif
