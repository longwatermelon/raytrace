#ifndef RENDER_H
#define RENDER_H

#include "sphere.h"
#include "util.h"

#include <sys/types.h>
#include <SDL2/SDL.h>

void render_rend();
Vec3f render_cast_ray(Vec3f o, Vec3f dir);
// hit: intersection point
// n: normal
bool render_scene_cast_ray(Vec3f o, Vec3f dir, Vec3f *hit, Vec3f *n);

void render_set_spheres(struct Sphere **spheres, size_t nspheres);
void render_set_lights(Light *lights, size_t nlights);
void render_set_dim(int x, int y);

#endif
