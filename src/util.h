#ifndef UTIL_H
#define UTIL_H

#include "vector.h"
#include <SDL2/SDL.h>

typedef struct
{
    Vec3f pos;
    float in; // intensity
} Light;

typedef struct
{
    Vec3f col;
    float specular_exp;

    float ref_diffuse, ref_specular, ref_mirror; // reflectiveness of diffuse, specular, and mirror (0.f - 1.f)
} Material;

enum
{
    OPT_BACKFACE_CULLING = 1
};

SDL_Point util_project_point(Vec3f p, int sw, int sh);
SDL_Point util_center_and_scale(SDL_FPoint p, int sw, int sh);

#endif
