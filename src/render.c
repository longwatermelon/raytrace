#include "render.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

int g_w, g_h;

struct Sphere **g_spheres;
size_t g_nspheres;

Light *g_lights;
size_t g_nlights;

void render_rend()
{
    Vec3f *frame = calloc(g_w * g_h, sizeof(Vec3f));

    FILE *fp = fopen("out.ppm", "w");
    fprintf(fp, "P3\n%d %d\n255\n", g_w, g_h);

    float fov = 1.f;

    printf("Casting rays\n");

    for (int y = 0; y < g_h; ++y)
    {
        for (int x = 0; x < g_w; ++x)
        {
            float ha = ((float)x / (float)g_w) * fov - (fov / 2.f);
            float va = ((float)y / (float)g_h) * fov - (fov / 2.f);

            float px = sinf(ha);
            float py = sinf(va);

            Vec3f dir = vec_normalize((Vec3f){ px, py, 1 });
            frame[y * g_w + x] = render_cast_ray((Vec3f){ 0.f, 0.f, 0.f }, dir);
        }
    }

    printf("Writing to file\n");

    for (int i = 0; i < g_w * g_h; ++i)
    {
        int r = 255.f * fmin(1.f, fmax(0.f, frame[i].x));
        int g = 255.f * fmin(1.f, fmax(0.f, frame[i].y));
        int b = 255.f * fmin(1.f, fmax(0.f, frame[i].z));

        fprintf(fp, "%d %d %d\n", r, g, b);
    }

    fclose(fp);
    printf("Done\n");
}


Vec3f render_cast_ray(Vec3f o, Vec3f dir)
{
    Vec3f hit, norm;

    if (!render_scene_cast_ray(o, dir, &hit, &norm))
        return (Vec3f){ .5f, .5f, .9f };

    float light = 0.f;

    for (size_t i = 0; i < g_nlights; ++i)
    {
        Vec3f v = vec_normalize(vec_sub(g_lights[i].pos, hit));
        light += g_lights[i].in * fmax(0.f, vec_mulv(v, norm));
    }

    return vec_mulf((Vec3f){ 1.f, 0.f, 0.f }, light);
}


bool render_scene_cast_ray(Vec3f o, Vec3f dir, Vec3f *hit, Vec3f *n)
{
    float nearest = INFINITY;

    for (size_t i = 0; i < g_nspheres; ++i)
    {
        float dist;

        if (sphere_ray_intersect(g_spheres[i], o, dir, &dist) && dist < nearest)
        {
            nearest = dist;
            *hit = vec_addv(o, vec_mulf(dir, dist));
            *n = vec_normalize(vec_sub(*hit, g_spheres[i]->c));
        }
    }

    return nearest < 1000.f;
}


void render_set_spheres(struct Sphere **spheres, size_t nspheres)
{
    g_spheres = spheres;
    g_nspheres = nspheres;
}


void render_set_lights(Light *lights, size_t nlights)
{
    g_lights = lights;
    g_nlights = nlights;
}


void render_set_dim(int x, int y)
{
    g_w = x;
    g_h = y;
}
