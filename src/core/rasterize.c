#include "rasterize.h"
#include "util.h"


Vec3f rasterize_rotate(Vec3f p, Vec3f angle)
{
    float rotx[3][3] = {
        { 1, 0, 0 },
        { 0, cosf(angle.y), sinf(angle.y) },
        { 0, -sinf(angle.y), cosf(angle.y) }
    };

    float roty[3][3] = {
        { cosf(angle.x), 0, sinf(angle.x) },
        { 0, 1, 0 },
        { -sinf(angle.x), 0, cosf(angle.x) }
    };

    p = util_matmul(roty, p);
    p = util_matmul(rotx, p);

    return p;
}


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

