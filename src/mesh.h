#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include <stdbool.h>
#include <sys/types.h>
#include <SDL2/SDL.h>

typedef struct
{
    int idx[3];
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
};

struct Mesh *mesh_alloc(Vec3f pos, const char *fp, Vec3f col);
void mesh_free(struct Mesh *m);

void mesh_read(struct Mesh *m, const char *fp);

// ro: ray origin
// t: nearest intersection
bool mesh_ray_intersect(struct Mesh *m, Vec3f ro, Vec3f rdir, float *t, Triangle *tri);
bool mesh_ray_tri_intersect(struct Mesh *m, Triangle tri, Vec3f ro, Vec3f rdir, float *t);

Vec3f mesh_tri_normal(struct Mesh *m, Triangle t);

#endif
