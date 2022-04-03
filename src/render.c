#include "render.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int g_w = 1000, g_h = 1000;
Vec3f g_bg = { .6f, .6f, .9f };

struct Sphere **g_spheres = 0;
size_t g_nspheres = 0;

struct Mesh **g_meshes = 0;
size_t g_nmeshes = 0;

Light *g_lights = 0;
size_t g_nlights = 0;

int g_max_bounces = 3;

size_t g_threads_finished = 0;
size_t g_rows_rendered = 0;

bool g_antialiasing = false;
size_t g_nthreads = 4;

Uint32 g_optimization = 0;

void render_rend()
{
    Vec3f *frame = malloc(sizeof(Vec3f) * (g_w * g_h));

    printf("Casting rays\n");

    pthread_t threads[g_nthreads];
    render_cast_rays_args args[g_nthreads];

    render_print_config();

    for (int i = 0; i < g_nthreads; ++i)
    {
        args[i] = (render_cast_rays_args){ .frame = frame, .y = i, .step = g_nthreads };
        pthread_create(&threads[i], 0, render_cast_rays, (void*)&args[i]);
    }

    while (g_threads_finished < g_nthreads)
    {
        render_print_progress();
        sleep(2);
    }

    for (size_t i = 0; i < g_nthreads; ++i)
        pthread_join(threads[i], 0);

    render_print_progress();

    printf("\n");
    
    if (g_antialiasing)
    {
        printf("Applying antialiasing\n");
        Vec3f *avg = render_apply_antialiasing(frame);
        free(frame);
        frame = avg;
    }

    printf("Writing to file\n");

    FILE *fp = fopen("out.ppm", "w");
    fprintf(fp, "P3\n%d %d\n255\n", g_w, g_h);

    for (int i = 0; i < g_w * g_h; ++i)
    {
        int r = 255.f * fmin(1.f, fmax(0.f, frame[i].x));
        int g = 255.f * fmin(1.f, fmax(0.f, frame[i].y));
        int b = 255.f * fmin(1.f, fmax(0.f, frame[i].z));

        fprintf(fp, "%d %d %d\n", r, g, b);
    }

    fclose(fp);
    printf("Done\n");

    free(frame);
}


void render_print_progress()
{
    printf("\r%zu rows rendered (%.2f%%)", g_rows_rendered,
            ((float)g_rows_rendered / g_h) * 100.f);
    fflush(stdout);
}


void render_print_config()
{
    printf("Output image dimensions: %dx%d\n", g_w, g_h);
    printf("%lu spheres, %lu meshes, %lu lights\n", g_nspheres, g_nmeshes, g_nlights);

    int rows_per_thread = g_h / g_nthreads;
    printf("%ld threads | %d rows per thread\n", g_nthreads, rows_per_thread);
    printf("Antialiasing %s\n", g_antialiasing ? "on" : "off");

    printf("Optimizations: ");
    if (g_optimization == 0) printf("none");
    if (g_optimization & OPT_BACKFACE_CULLING) printf("backface culling");
    
    printf("\n");
}


void *render_cast_rays(void *arg)
{
    render_cast_rays_args *args = (render_cast_rays_args*)arg;
    float fov = 1.f;

    for (int y = args->y; y < g_h; y += args->step)
    {
        for (int x = 0; x < g_w; ++x)
        {
            float ha = ((float)x / (float)g_w) * fov - (fov / 2.f);
            float va = ((float)y / (float)g_h) * fov - (fov / 2.f);

            float px = sinf(ha);
            float py = sinf(va);

            Vec3f dir = vec_normalize((Vec3f){ px, py, 1 });
            args->frame[y * g_w + x] = render_cast_ray((Vec3f){ 0.f, 0.f, 0.f }, dir, true, 0);
        }

        ++g_rows_rendered;
    }
        
    ++g_threads_finished;
    pthread_exit(0);
}


Vec3f render_cast_ray(Vec3f o, Vec3f dir, bool optimize_meshes, int bounce)
{
    Vec3f hit, norm;
    Material mat;

    if (!render_scene_cast_ray(o, dir, optimize_meshes, &hit, &norm, &mat))
        return g_bg;

    float dlight = 0.f;
    float slight = 0.f;

    for (size_t i = 0; i < g_nlights; ++i)
    {
        // shadow
        Vec3f orig = vec_addv(hit, vec_divf(norm, 1e3f));
        Vec3f sdir = vec_normalize(vec_sub(g_lights[i].pos, orig));
        
        Vec3f shadow_hit, shadow_norm;
        Material shadow_mat;
        if (render_scene_cast_ray(orig, sdir, false, &shadow_hit, &shadow_norm, &shadow_mat))
        {
            if (vec_len(vec_sub(shadow_hit, hit)) <= vec_len(vec_sub(g_lights[i].pos, hit)))
                continue;
        }

        // diffuse
        Vec3f l = vec_normalize(vec_sub(g_lights[i].pos, hit));
        dlight += g_lights[i].in * fmax(0.f, vec_mulv(l, norm));

        // specular
        Vec3f r = vec_sub(l, vec_mulf(vec_mulf(norm, 2.f), vec_mulv(l, norm)));
        slight += powf(fmax(0.f, vec_mulv(r, vec_normalize(hit))), mat.specular_exp);
    }

    // mirror reflection
    Vec3f morig = vec_addv(hit, vec_divf(norm, 1e3f));

    Vec3f hcol = vec_addf(vec_mulf(vec_mulf(mat.col, dlight), mat.ref_diffuse), slight * mat.ref_specular);

    if (mat.ref_mirror < 1.f && bounce < g_max_bounces)
    {
        Vec3f col = render_cast_ray(morig, norm, false, bounce + 1);
        hcol = vec_mulf(vec_addv(hcol, vec_mulf(col, .6f)), mat.ref_mirror);
    }

    return hcol;
}


bool render_scene_cast_ray(Vec3f o, Vec3f dir, bool optimize_meshes, Vec3f *hit, Vec3f *n, Material *mat)
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
            *mat = g_spheres[i]->mat;
        }
    }

    for (size_t i = 0; i < g_nmeshes; ++i)
    {
        if (optimize_meshes &&
            (dir.y < g_meshes[i]->top_ry || dir.y > g_meshes[i]->bot_ry ||
            dir.x < g_meshes[i]->left_rx || dir.x > g_meshes[i]->right_rx))
            continue;

        float dist;
        Triangle tri;

        if (mesh_ray_intersect(g_meshes[i], o, dir, g_optimization, &dist, &tri) && dist < nearest)
        {
            nearest = dist;
            *hit = vec_addv(o, vec_mulf(dir, dist));
            *n = g_meshes[i]->norms[tri.nidx];
            *mat = g_meshes[i]->mat;
        }
    }

    return nearest < 1000.f;
}


Vec3f *render_apply_antialiasing(Vec3f *frame)
{
    size_t size = sizeof(Vec3f) * (g_w * g_h);
    Vec3f *avg = malloc(size);
    memcpy(avg, frame, size);

    for (int y = 1; y < g_h - 1; ++y)
    {
        for (int x = 1; x < g_w - 1; ++x)
        {
            avg[y * g_w + x] = render_smoothen(frame, x, y);
        }
    }

    return avg;
}


Vec3f render_smoothen(Vec3f *frame, int cx, int cy)
{
    Vec3f avg = { 0.f, 0.f, 0.f };

    float c = 1.f / 45.f;
    for (int y = -1; y <= 1; y += 2)
        for (int x = -1; x <= 1; x += 2)
            avg = vec_addv(avg, vec_mulf(frame[(cy + y) * g_w + (cx + x)], c));

    c = 4.f / 45.f;
    avg = vec_addv(avg, vec_mulf(frame[(cy - 1) * g_w + cx], c));
    avg = vec_addv(avg, vec_mulf(frame[cy * g_w + (cx + 1)], c));
    avg = vec_addv(avg, vec_mulf(frame[cy * g_w + (cx - 1)], c));
    avg = vec_addv(avg, vec_mulf(frame[(cy + 1) * g_w + cx], c));

    avg = vec_addv(avg, vec_mulf(frame[cy * g_w + cx], 5.f / 9.f));
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

void render_set_bg(Vec3f col)
{
    g_bg = col;
}

void render_set_max_bounces(int i)
{
    g_max_bounces = i;
}

void render_enable_antialiasing()
{
    g_antialiasing = true;
}

void render_set_threads(int threads)
{
    g_nthreads = threads;
}

void render_enable_optimizations(Uint32 flag)
{
    g_optimization |= flag;
}

void render_free_objects()
{
    for (size_t i = 0; i < g_nspheres; ++i)
        sphere_free(g_spheres[i]);

    free(g_spheres);

    for (size_t i = 0; i < g_nmeshes; ++i)
        mesh_free(g_meshes[i]);

    free(g_meshes);

    free(g_lights);
}
