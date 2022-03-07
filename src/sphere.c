#include "sphere.h"
#include <math.h>
#include <stdlib.h>


struct Sphere *sphere_alloc(Vec3f c, float r)
{
    struct Sphere *s = malloc(sizeof(struct Sphere));
    s->c = c;
    s->r = r;

    return s;
}


void sphere_free(struct Sphere *s)
{
    free(s);
}


bool sphere_ray_intersect(struct Sphere *s, Vec3f ro, Vec3f rdir, float *t)
{
    Vec3f co = vec_sub(ro, s->c);

    float a = vec_mulv(rdir, rdir);
    float b = vec_mulv(co, rdir) * 2.f;
    float c = vec_mulv(co, co) - s->r * s->r;

    float discrim = b * b - 4.f * a * c;

    if (discrim < 0)
        return false;

    float t1 = (-b * sqrtf(discrim)) / (2.f * a);
    float t2 = (b * sqrtf(discrim)) / (2.f * a);

    // account for floating point errors
    if (t1 <= 1e-4f)
        return false;

    if (t2 <= 1e-4f)
        *t = t1;
    else
        *t = (t1 < t2 ? t1 : t2); // nearest intersection

    return true;
}
