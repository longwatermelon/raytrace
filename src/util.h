#ifndef UTIL_H
#define UTIL_H

#include "vector.h"
#include <SDL2/SDL.h>

#define LOG(x, ...) { if (x <= util_loglevel()) printf(__VA_ARGS__); }

enum { LOG_SILENT, LOG_NORMAL, LOG_VERBOSE };

enum
{
    OPT_BACKFACE_CULLING = 1
};

SDL_Point util_project_point(Vec3f p, int sw, int sh);
SDL_Point util_center_and_scale(SDL_FPoint p, int sw, int sh);

void util_set_loglevel(int level);
int util_loglevel();

#endif
