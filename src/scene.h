#ifndef SCENE_H
#define SCENE_H

#include "sphere.h"
#include "mesh.h"
#include "util.h"

struct Scene
{
    struct Sphere **spheres;
    size_t nspheres;

    struct Mesh **meshes;
    size_t nmeshes;

    Light *lights;
    size_t nlights;
};

struct Scene *scene_alloc(struct Sphere **spheres, size_t nspheres, struct Mesh **meshes, size_t nmeshes, Light *lights, size_t nlights);
void scene_free(struct Scene *s);

#endif

