#ifndef SCENE_H
#define SCENE_H

#include "sphere.h"
#include "mesh.h"
#include "texmap.h"
#include "util.h"

struct Scene
{
    size_t w, h;
    Vec3f bg;

    struct Sphere **spheres;
    size_t nspheres;

    struct Mesh **meshes;
    size_t nmeshes;

    struct Light **lights;
    size_t nlights;

    struct Material **mats;
    size_t nmats;

    struct Texmap **texs;
    size_t ntexs;

    Vec3f cam;
};

struct Scene *scene_alloc(const char *fp);
void scene_free(struct Scene *s);

struct Sphere *scene_parse_sphere(struct Scene *sc, char *s);
struct Mesh *scene_parse_mesh(struct Scene *sc, char *s);
struct Light *scene_parse_light(struct Scene *sc, char *s);
struct Texmap *scene_parse_tex(struct Scene *sc, char *s);

#endif

