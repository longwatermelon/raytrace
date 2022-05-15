#include "rasterize.h"


SDL_Point rasterize_project_point(Vec3f p, int sw, int sh)
{
    SDL_FPoint proj = {
        p.x / p.z,
        p.y / p.z
    };

    return rasterize_center_and_scale(proj, sw, sh);
}


SDL_Point rasterize_center_and_scale(SDL_FPoint p, int sw, int sh)
{
    return (SDL_Point){
        (p.x + 1.f) * ((float)sw / 2.f),
        (p.y + 1.f) * ((float)sh / 2.f)
    };
}

