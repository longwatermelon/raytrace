#ifndef CORE_MESH_H
#define CORE_MESH_H

#include "vector.h"
#include "util.h"
#include "light.h"
#include "camera.h"
#include <stdbool.h>
#include <sys/types.h>

typedef struct
{
    int idx[3];
    int nidx;
} Triangle;

struct Mesh
{
    Vec3f pos, rot;

    Vec3f *pts;
    size_t npts;

    Triangle *tris;
    size_t ntris;
    
    Vec3f *norms;
    size_t nnorms;

    struct Material *mat;

    // optimization
    // pixels on screen
    Point min, max;

    char name[PATH_MAX];
    bool bounded;

    SDL_Color col;
};

struct Mesh *mesh_alloc(Vec3f pos, Vec3f rot, const char *fp, struct Material *mat);
void mesh_free(struct Mesh *m);

void mesh_read(struct Mesh *m, const char *fp);

// ro: ray origin
// t: nearest intersection
bool mesh_ray_intersect(struct Mesh *m, Vec3f ro, Vec3f rdir, Uint32 opt, float *t, Triangle *tri);
bool mesh_ray_tri_intersect(struct Mesh *m, Triangle tri, Vec3f ro, Vec3f rdir, float *t);

void mesh_find_bounds(struct Mesh *m, struct Camera *cam);

#endif
