#include "scene.h"


struct Scene *scene_alloc(struct Sphere **spheres, size_t nspheres, struct Mesh **meshes, size_t nmeshes, Light *lights, size_t nlights)
{
    struct Scene *s = malloc(sizeof(struct Scene));
    s->spheres = spheres;
    s->nspheres = nspheres;

    s->meshes = meshes;
    s->nmeshes = nmeshes;

    s->lights = lights;
    s->nlights = nlights;

    return s;
}


void scene_free(struct Scene *s)
{
    for (size_t i = 0; i < s->nspheres; ++i)
        sphere_free(s->spheres[i]);

    for (size_t i = 0; i < s->nmeshes; ++i)
        mesh_free(s->meshes[i]);

    free(s->lights);
    free(s);
}

