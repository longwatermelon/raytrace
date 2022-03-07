#include "render.h"
#include <stdio.h>
#include <stdlib.h>

int g_w, g_h;

struct Sphere **g_spheres;
size_t g_nspheres;

void render_rend()
{
    SDL_Color frame[g_w * g_h];

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
        // int r = (int)(255.f * fmax(0.f, fmin(1.f, frame[i].r)));
        // int g = (int)(255.f * fmax(0.f, fmin(1.f, frame[i].g)));
        // int b = (int)(255.f * fmax(0.f, fmin(1.f, frame[i].b)));

        int r = frame[i].r;
        int g = frame[i].g;
        int b = frame[i].b;

        fprintf(fp, "%d %d %d\n", r, g, b);
    }

    fclose(fp);
    printf("Done\n");
}


SDL_Color render_cast_ray(Vec3f o, Vec3f dir)
{
    float t;

    if (sphere_ray_intersect(g_spheres[0], o, dir, &t))
        return (SDL_Color){ 255, 100, 100 };
    else
        return (SDL_Color){ 100, 100, 255 };
}


void render_set_spheres(struct Sphere **spheres, size_t nspheres)
{
    g_spheres = spheres;
    g_nspheres = nspheres;
}


void render_set_dim(int x, int y)
{
    g_w = x;
    g_h = y;
}
