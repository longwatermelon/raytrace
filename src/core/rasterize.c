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

    float rotz[3][3] = {
        { cosf(angle.z), sinf(angle.z), 0 },
        { -sinf(angle.z), cosf(angle.z), 0 },
        { 0, 0, 1 }
    };

    return util_matmul(rotz, util_matmul(rotx, util_matmul(roty, p)));
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
        (p.x + .5f) * ((float)sw),
        (p.y + .5f) * ((float)sh)
    };
}

