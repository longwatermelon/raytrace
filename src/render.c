#include "render.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int g_w, g_h;

struct Sphere **g_spheres;
size_t g_nspheres;

struct Mesh **g_meshes;
size_t g_nmeshes;

Light *g_lights;
size_t g_nlights;

void render_rend()
{
    Vec3f *frame = malloc(sizeof(Vec3f) * (g_w * g_h));

    FILE *fp = fopen("out.ppm", "w");
    fprintf(fp, "P3\n%d %d\n255\n", g_w, g_h);

    float fov = 1.f;

    printf("Casting rays\n");

    struct timespec t1, now;
    clock_gettime(CLOCK_MONOTONIC, &t1);

    for (int y = 0; y < g_h; ++y)
    {
        for (int x = 0; x < g_w; ++x)
        {
            float ha = ((float)x / (float)g_w) * fov - (fov / 2.f);
            float va = ((float)y / (float)g_h) * fov - (fov / 2.f);

            float px = sinf(ha);
            float py = sinf(va);

            Vec3f dir = vec_normalize((Vec3f){ px, py, 1 });
            frame[y * g_w + x] = render_cast_ray((Vec3f){ 0.f, -2.f, 0.f }, dir);
        }

        clock_gettime(CLOCK_MONOTONIC, &now);

        if (now.tv_sec - t1.tv_sec >= 5)
        {
            printf("\r%d%% done", (int)(((float)y / g_h) * 100));
            fflush(stdout);
            clock_gettime(CLOCK_MONOTONIC, &t1);
        }
    }

    printf("\n");
    printf("Applying antialiasing\n");

    Vec3f *avg = render_apply_antialiasing(frame);

    printf("Writing to file\n");

    for (int i = 0; i < g_w * g_h; ++i)
    {
        int r = 255.f * fmin(1.f, fmax(0.f, avg[i].x));
        int g = 255.f * fmin(1.f, fmax(0.f, avg[i].y));
        int b = 255.f * fmin(1.f, fmax(0.f, avg[i].z));

        fprintf(fp, "%d %d %d\n", r, g, b);
    }

    fclose(fp);
    printf("Done\n");

    free(frame);
    free(avg);
}


Vec3f render_cast_ray(Vec3f o, Vec3f dir)
{
    Vec3f hit, norm;

    if (!render_scene_cast_ray(o, dir, &hit, &norm))
        return (Vec3f){ .5f, .5f, .9f };

    float dlight = 0.f;
    float slight = 0.f;

    for (size_t i = 0; i < g_nlights; ++i)
    {
        // shadow
        Vec3f orig = vec_addv(hit, vec_divf(norm, 1e3f));
        Vec3f sdir = vec_normalize(vec_sub(g_lights[i].pos, orig));
        
        Vec3f shadow_hit, shadow_norm;
        if (render_scene_cast_ray(orig, sdir, &shadow_hit, &shadow_norm))
            continue;

        // diffuse
        Vec3f l = vec_normalize(vec_sub(g_lights[i].pos, hit));
        dlight += g_lights[i].in * fmax(0.f, vec_mulv(l, norm));

        // specular
        Vec3f r = vec_sub(l, vec_mulf(vec_mulf(norm, 2.f), vec_mulv(l, norm)));
        slight += powf(fmax(0.f, vec_mulv(r, vec_normalize(hit))), 50.f);
    }

    // dlight = fminf(dlight, 1.f);
    // slight = fminf(slight, 1.f);
    return vec_addf(vec_mulf((Vec3f){ 1.f, 0.7f, 0.95f }, dlight), slight);
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

    for (size_t i = 0; i < g_nmeshes; ++i)
    {
        float dist;
        Triangle tri;

        if (mesh_ray_intersect(g_meshes[i], o, dir, &dist, &tri) && dist < nearest)
        {
            nearest = dist;
            *hit = vec_addv(o, vec_mulf(dir, dist));
            *n = vec_normalize(mesh_tri_normal(g_meshes[i], tri));
        }
    }

    return nearest < 1000.f;
}


Vec3f *render_apply_antialiasing(Vec3f *frame)
{
    Vec3f *avg = malloc(sizeof(Vec3f) * (g_w * g_h));

    for (int y = 1; y < g_h - 1; ++y)
    {
        for (int x = 1; x < g_w - 1; ++x)
        {
            Vec3f a = { 0.f, 0.f, 0.f };
            
            for (int i = y - 1; i <= y + 1; ++i)
            {
                for (int j = x - 1; j <= x + 1; ++j)
                {
                    a.x += frame[i * g_w + j].x;
                    a.y += frame[i * g_w + j].y;
                    a.z += frame[i * g_w + j].z;
                }
            }

            a.x /= 9.f;
            a.y /= 9.f;
            a.z /= 9.f;

            avg[y * g_w + x] = a;
        }
    }

    return avg;
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


void render_set_meshes(struct Mesh **meshes, size_t nmeshes)
{
    g_meshes = meshes;
    g_nmeshes = nmeshes;
}


void render_set_dim(int x, int y)
{
    g_w = x;
    g_h = y;
}
