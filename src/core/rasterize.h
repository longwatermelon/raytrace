#ifndef CORE_RASTERIZE_H
#define CORE_RASTERIZE_H

#include "util.h"
#include <SDL2/SDL.h>

Vec3f rasterize_rotate_ccw(Vec3f p, Vec3f angle);
Vec3f rasterize_rotate_cc(Vec3f p, Vec3f angle);
SDL_Point rasterize_project_point(Vec3f p, int sw, int sh);
SDL_Point rasterize_center_and_scale(SDL_FPoint p, int sw, int sh);

#endif

