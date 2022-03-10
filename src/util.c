#include "util.h"


SDL_Point util_project_point(Vec3f p, int sw, int sh)
{
    SDL_FPoint proj = {
        p.x / p.z,
        p.y / p.z
    };

    return util_center_and_scale(proj, sw, sh);
}


SDL_Point util_center_and_scale(SDL_FPoint p, int sw, int sh)
{
    return (SDL_Point){
        (p.x + 1.f) * ((float)sw / 2.f),
        (p.y + 1.f) * ((float)sh / 2.f)
    };
}
