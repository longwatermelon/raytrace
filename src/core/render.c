#include "render.h"
#include "rasterize.h"
#include "texmap.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int g_max_bounces = 3;

bool g_antialiasing = false;
size_t g_nthreads = 4;

Uint32 g_optimization = 0;

float g_progress = 0.f;
int g_sleep_time = 1;

Vec3f *render_rend(struct Scene *sc)
{
    g_progress = 0.f;

    LOG(LOG_VERBOSE, "Casting rays\n");
    render_print_config(sc);

    Vec3f *frame = render_rend_cast_rays(sc);

    if (g_antialiasing)
    {
        LOG(LOG_VERBOSE, "Applying antialiasing\n");
        Vec3f *avg = render_apply_antialiasing(sc, frame);
        free(frame);
        frame = avg;
    }

    return frame;
}


Vec3f *render_rend_cast_rays(struct Scene *sc)
{
    Vec3f *frame = malloc(sizeof(Vec3f) * (sc->w * sc->h));

    pthread_t threads[g_nthreads];
    render_cast_rays_args *args[g_nthreads];

    size_t rows_rendered[g_nthreads];
    bool threads_done[g_nthreads];

    for (int i = 0; i < g_nthreads; ++i)
    {
        rows_rendered[i] = 0;
        threads_done[i] = false;

        args[i] = malloc(sizeof(render_cast_rays_args));
        args[i]->sc = sc;
        args[i]->frame = frame;
        args[i]->y = i;
        args[i]->step = g_nthreads;
        args[i]->rows_rendered = &rows_rendered[i];
        args[i]->done = &threads_done[i];

        pthread_create(&threads[i], 0, render_cast_rays, (void*)args[i]);
    }

    render_rend_wait_cthreads(args);

    for (size_t i = 0; i < g_nthreads; ++i)
    {
        pthread_join(threads[i], 0);
        free(args[i]);
    }

    return frame;
}


void render_rend_wait_cthreads(render_cast_rays_args **args)
{
    struct Scene *sc = args[0]->sc;

    while (true)
    {
        bool done = true;
        size_t rows_rendered = 0;

        for (size_t i = 0; i < g_nthreads; ++i)
        {
            if (!*args[i]->done)
                done = false;

            rows_rendered += *args[i]->rows_rendered;
        }

        render_print_progress(sc, rows_rendered);
        g_progress = (float)rows_rendered / sc->h;

        if (done)
            break;

        sleep(g_sleep_time);
    }

    LOG(LOG_NORMAL, "\n");
}


void render_write_to_file(struct Scene *sc, Vec3f *frame, const char *out)
{
    FILE *fp = fopen(out, "w");
    fprintf(fp, "P3\n%zu %zu\n255\n", sc->w, sc->h);

    for (int i = 0; i < sc->w * sc->h; ++i)
    {
        int r = 255.f * fmin(1.f, fmax(0.f, frame[i].x));
        int g = 255.f * fmin(1.f, fmax(0.f, frame[i].y));
        int b = 255.f * fmin(1.f, fmax(0.f, frame[i].z));

        fprintf(fp, "%d %d %d\n", r, g, b);
    }

    fclose(fp);
}


void render_print_progress(struct Scene *sc, size_t rows_rendered)
{
    if (util_loglevel() >= LOG_NORMAL)
    {
        printf("\r%zu rows rendered (%.2f%%)", rows_rendered,
                ((float)rows_rendered / sc->h) * 100.f);
        fflush(stdout);
    }
}


float render_get_progress()
{
    return g_progress;
}


void render_set_progress(float p)
{
    g_progress = p;
}

void render_print_config(struct Scene *sc)
{
    if (util_loglevel() == LOG_VERBOSE)
    {
        printf("Output image dimensions: %zux%zu\n", sc->w, sc->h);
        printf("%lu spheres, %lu meshes, %lu lights\n", sc->nspheres, sc->nmeshes, sc->nlights);

        int rows_per_thread = sc->h / g_nthreads;
        printf("%ld threads | %d rows per thread\n", g_nthreads, rows_per_thread);
        printf("Antialiasing %s\n", g_antialiasing ? "on" : "off");

        printf("Optimizations: ");
        if (g_optimization == 0) printf("none");
        if (g_optimization & OPT_BACKFACE_CULLING) printf("backface culling");
        
        printf("\n");
    }
}


void *render_cast_rays(void *arg)
{
    render_cast_rays_args *args = (render_cast_rays_args*)arg;
    float fov = 1.f;

    for (int y = args->y; y < args->sc->h; y += args->step)
    {
        for (int x = 0; x < args->sc->w; ++x)
        {
            float ha = ((float)x / (float)args->sc->w) * fov - (fov / 2.f);
            float va = ((float)y / (float)args->sc->h) * fov - (fov / 2.f);

            float px = sinf(ha);
            float py = sinf(va);

            Vec3f dir = vec_normalize((Vec3f){ px, py, 1 });
            dir = rasterize_rotate_cc(dir, args->sc->cam->angle);
            args->frame[y * args->sc->w + x] = render_cast_ray(args->sc, args->sc->cam->pos, dir, (Point){ x, y }, true, 0);
        }

        ++*args->rows_rendered;
    }
        
    *args->done = true;
    pthread_exit(0);
}


Vec3f render_cast_ray(struct Scene *sc, Vec3f o, Vec3f dir, Point pixel, bool optimize_meshes, int bounce)
{
    Vec3f hit, norm;
    void *obj;
    int obj_type;

    if (!render_scene_cast_ray(sc, o, dir, pixel, optimize_meshes, &hit, &norm, &obj, &obj_type))
        return sc->bg;

    struct Material *mat = 0;

    switch (obj_type)
    {
    case OBJ_MESH: mat = ((struct Mesh*)obj)->mat; break;
    case OBJ_SPHERE: mat = ((struct Sphere*)obj)->mat; break;
    }

    float dlight = 0.f;
    float slight = 0.f;

    for (size_t i = 0; i < sc->nlights; ++i)
    {
        // shadow
        Vec3f orig = vec_addv(hit, vec_divf(norm, 1e3f));
        Vec3f sdir = vec_normalize(vec_sub(sc->lights[i]->pos, orig));

        Vec3f shadow_hit;
        if (render_scene_cast_ray(sc, orig, sdir, pixel, false, &shadow_hit, 0, 0, 0))
        {
            if (vec_len(vec_sub(shadow_hit, hit)) <= vec_len(vec_sub(sc->lights[i]->pos, hit)))
                continue;
        }

        // diffuse
        Vec3f l = vec_normalize(vec_sub(sc->lights[i]->pos, hit));
        dlight += sc->lights[i]->in * fmax(0.f, vec_dot(l, norm));

        // specular
        Vec3f r = vec_sub(l, vec_mulf(vec_mulf(norm, 2.f), vec_dot(l, norm)));
        slight += powf(fmax(0.f, vec_dot(r, vec_normalize(hit))), mat->specular_exp);
    }

    // mirror reflection
    Vec3f morig = vec_addv(hit, vec_divf(norm, 1e3f));

    Vec3f mcol = (mat->tex ? tex_color(mat->tex, morig, norm, mat->col) : mat->col);
    Vec3f hcol = vec_addf(vec_mulf(vec_mulf(mcol, dlight), mat->ref_diffuse), slight * mat->ref_specular);

    if (mat->ref_mirror < 1.f && bounce < g_max_bounces)
    {
        Vec3f col = render_cast_ray(sc, morig, norm, pixel, false, bounce + 1);
        hcol = vec_addv(vec_mulf(hcol, mat->ref_mirror), vec_mulf(col, 1.f - mat->ref_mirror));
    }

    return hcol;
}


bool render_scene_cast_ray(struct Scene *sc, Vec3f o, Vec3f dir, Point pixel, bool optimize_meshes, Vec3f *hit, Vec3f *n, void **obj, int *obj_type)
{
    float nearest = INFINITY;

    for (size_t i = 0; i < sc->nspheres; ++i)
    {
        float dist;

        if (sphere_ray_intersect(sc->spheres[i], o, dir, &dist) && dist < nearest)
        {
            nearest = dist;
            if (hit) *hit = vec_addv(o, vec_mulf(dir, dist));
            if (n) *n = vec_normalize(vec_sub(*hit, sc->spheres[i]->c));
            if (obj) *obj = sc->spheres[i];
            if (obj_type) *obj_type = OBJ_SPHERE;
        }
    }

    for (size_t i = 0; i < sc->nmeshes; ++i)
    {
        if (optimize_meshes &&
            (pixel.y < sc->meshes[i]->min.y || pixel.y > sc->meshes[i]->max.y ||
            pixel.x < sc->meshes[i]->min.x || pixel.x > sc->meshes[i]->max.x))
            continue;

        float dist;
        Triangle tri;

        if (mesh_ray_intersect(sc->meshes[i], o, dir, g_optimization, &dist, &tri) && dist < nearest)
        {
            nearest = dist;
            if (hit) *hit = vec_addv(o, vec_mulf(dir, dist));
            if (n) *n = rasterize_rotate_cc(sc->meshes[i]->norms[tri.nidx], sc->meshes[i]->rot);
            if (obj) *obj = (void*)sc->meshes[i];
            if (obj_type) *obj_type = OBJ_MESH;
        }
    }

    return nearest < 1000.f;
}


Vec3f *render_apply_antialiasing(struct Scene *sc, Vec3f *frame)
{
    size_t size = sizeof(Vec3f) * (sc->w * sc->h);
    Vec3f *avg = malloc(size);
    memcpy(avg, frame, size);

    for (int y = 1; y < sc->h - 1; ++y)
    {
        for (int x = 1; x < sc->w - 1; ++x)
        {
            avg[y * sc->w + x] = render_smoothen(sc, frame, x, y);
        }
    }

    return avg;
}


Vec3f render_smoothen(struct Scene *sc, Vec3f *frame, int cx, int cy)
{
    Vec3f avg = { 0.f, 0.f, 0.f };

    float c = 1.f / 45.f;
    for (int y = -1; y <= 1; y += 2)
        for (int x = -1; x <= 1; x += 2)
            avg = vec_addv(avg, vec_mulf(frame[(cy + y) * sc->w + (cx + x)], c));

    c = 4.f / 45.f;
    avg = vec_addv(avg, vec_mulf(frame[(cy - 1) * sc->w + cx], c));
    avg = vec_addv(avg, vec_mulf(frame[cy * sc->w + (cx + 1)], c));
    avg = vec_addv(avg, vec_mulf(frame[cy * sc->w + (cx - 1)], c));
    avg = vec_addv(avg, vec_mulf(frame[(cy + 1) * sc->w + cx], c));

    avg = vec_addv(avg, vec_mulf(frame[cy * sc->w + cx], 5.f / 9.f));
    return avg;
}

void render_set_max_bounces(int i) { g_max_bounces = i; }
void render_enable_antialiasing() { g_antialiasing = true; }
void render_set_threads(int threads) { g_nthreads = threads; }
void render_enable_optimizations(Uint32 flag) { g_optimization |= flag; }
void render_set_sleep(int time) { g_sleep_time = time; }
