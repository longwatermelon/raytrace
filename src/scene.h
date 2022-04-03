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

    Material *mats;
    size_t nmats;
};

struct Scene *scene_alloc(const char *fp);
void scene_free(struct Scene *s);

struct Sphere *scene_parse_sphere(char *s, Material *mats);
struct Mesh *scene_parse_mesh(char *s, Material *mats);
Light scene_parse_light(char *s);

#endif
