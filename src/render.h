#ifndef RENDER_H
#define RENDER_H

#include "sphere.h"
#include <sys/types.h>
#include <SDL2/SDL.h>

void render_rend();
SDL_Color render_cast_ray(Vec3f o, Vec3f dir);

void render_set_spheres(struct Sphere **spheres, size_t nspheres);
void render_set_dim(int x, int y);

#endif
