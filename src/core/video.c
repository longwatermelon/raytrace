#include "video.h"
#include "mesh.h"
#include "scene.h"
#include "render.h"
#include "sphere.h"
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <sys/stat.h>


struct VideoEvent *ve_alloc(struct Scene *sc, char *s)
{
    struct VideoEvent *ve = malloc(sizeof(struct VideoEvent));

    int obj_idx;
    sscanf(s, "%*s %d|%zu %zu|%d", &ve->type, &ve->fbegin, &ve->fend, &obj_idx);

    ve->delta = ve_parse_obj(sc, s, ve->type);

    switch (ve->type)
    {
    case OBJ_SPHERE: ve->obj = (void*)sc->spheres[obj_idx]; break;
    case OBJ_MESH: ve->obj = (void*)sc->meshes[obj_idx]; break;
    case OBJ_LIGHT: ve->obj = (void*)sc->lights[obj_idx]; break;
    case OBJ_TEX: ve->obj = (void*)sc->texs[obj_idx]; break;
    default: ve->obj = 0; break;
    }

    return ve;
}


void ve_free(struct VideoEvent *ve)
{
    // obj is non-owning, don't free

    switch (ve->type)
    {
    case OBJ_SPHERE: sphere_free(ve->delta); break;
    case OBJ_MESH: mesh_free(ve->delta); break;
    case OBJ_LIGHT: light_free(ve->delta); break;
    case OBJ_TEX: tex_free(ve->delta); break;
    }

    free(ve);
}


void *ve_parse_obj(struct Scene *sc, char *line, int type)
{
    char *s = line;
    while (*s != '>') ++s;
    ++s;

    switch (type)
    {
    case OBJ_SPHERE: return (void*)scene_parse_sphere(sc, s);
    case OBJ_MESH: return (void*)scene_parse_mesh(sc, s);
    case OBJ_LIGHT: return (void*)scene_parse_light(sc, s);
    case OBJ_TEX: return (void*)scene_parse_tex(sc, s);
    }

    return 0;
}


struct Video *video_alloc(const char *config)
{
    struct Video *v = malloc(sizeof(struct Video));
    v->base = 0;
    v->events = 0;
    v->nevents = 0;
    v->nframes = 0;

    video_load_config(v, config);

    return v;
}


void video_free(struct Video *v)
{
    for (size_t i = 0; i < v->nevents; ++i)
        ve_free(v->events[i]);

    free(v->events);
    scene_free(v->base);

    free(v);
}


void video_load_config(struct Video *v, const char *config)
{
    FILE *fp = fopen(config, "r");

    char *line = 0;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, fp)) != -1)
    {
        char word[11] = { 0 };
        sscanf(line, "%10s", word);

        if (strncmp(word, "//", 2) == 0)
        {
            continue;
        }
        else if (strcmp(word, "base") == 0)
        {
            char base_fp[PATH_MAX] = { 0 };
            sscanf(line, "%*s %s", base_fp);
            v->base = scene_alloc(base_fp);
        }
        else if (strcmp(word, "frames") == 0)
        {
            sscanf(line, "%*s %zu", &v->nframes);
        }
        else if (strcmp(word, "evt") == 0)
        {
            if (!v->base)
            {
                fprintf(stderr, "ERROR: Base scene not loaded in file '%s'.\n", config);
                exit(EXIT_FAILURE);
            }

            v->events = realloc(v->events, sizeof(struct VideoEvent*) * ++v->nevents);
            v->events[v->nevents - 1] = ve_alloc(v->base, line);
        }
    }

    free(line);
    fclose(fp);
}


void video_create(struct Video *v)
{
    system("rm -rf frames out.mp4");
    mkdir("frames", 0777);

    for (size_t i = 0; i < v->nframes; ++i)
    {
        LOG(LOG_NORMAL, "== Frame %zu (%.2f%%) ==\n", i + 1, ((float)(i + 1) / (float)v->nframes) * 100.f);

        for (size_t j = 0; j < v->nevents; ++j)
        {
            struct VideoEvent *evt = v->events[j];

            if (i >= evt->fbegin && i <= evt->fend)
                video_apply_delta(v, evt->obj, evt->delta, evt->type);
        }

        Vec3f *frame = render_rend(v->base);
        render_write_to_file(v->base, frame, "out.ppm");
        free(frame);

        char cmd[100] = { 0 };
        sprintf(cmd, "ffmpeg -i out.ppm frames/%zu.png &> /dev/null", i);
        system(cmd);
    }

    system("ffmpeg -framerate 25 -i frames/%d.png -c:v libx264 -pix_fmt yuv420p out.mp4 &> /dev/null");
}


void video_apply_delta(struct Video *v, void *obj, void *delta, int type)
{
    switch (type)
    {
    case OBJ_SPHERE:
    {
        struct Sphere *sphere = (struct Sphere*)obj;
        struct Sphere *d = (struct Sphere*)delta;

        sphere->c = vec_addv(sphere->c, d->c);
        sphere->r += d->r;
    } break;
    case OBJ_MESH:
    {
        struct Mesh *m = (struct Mesh*)obj;
        struct Mesh *d = (struct Mesh*)delta;

        m->pos = vec_addv(m->pos, d->pos);
        m->rot = vec_addv(m->rot, d->rot);
        mesh_find_bounds(m, v->base->cam);
    } break;
    case OBJ_LIGHT:
    {
        struct Light *l = (struct Light*)obj;
        struct Light *d = (struct Light*)delta;

        l->pos = vec_addv(l->pos, d->pos);
        l->in += d->in;
    } break;
    case OBJ_TEX:
    {
        struct Texmap *t = (struct Texmap*)obj;
        struct Texmap *d = (struct Texmap*)delta;

        t->pos = vec_addv(t->pos, d->pos);
        t->w += d->w;
        t->h += d->h;
    } break;
    }
}

