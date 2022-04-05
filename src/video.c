#include "video.h"
#include "mesh.h"
#include "scene.h"
#include "render.h"
#include <string.h>
#include <stdio.h>
#include <limits.h>


struct VideoEvent *ve_alloc(int type, void *obj, void *delta, size_t fbegin, size_t fcount)
{
    struct VideoEvent *ve = malloc(sizeof(struct VideoEvent));
    ve->type = type;
    ve->obj = obj;
    ve->delta = delta;

    ve->fbegin = fbegin;
    ve->fcount = fcount;

    return ve;
}


void ve_free(struct VideoEvent *ve)
{
    free(ve);
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

            int vtype;
            int obj_idx;
            size_t fbegin, fcount;

            sscanf(line, "%*s %d|%zu %zu|%d", &vtype, &fbegin, &fcount, &obj_idx);
            void *delta = video_parse_obj(v, line, vtype);

            void *obj;
            switch (vtype)
            {
            case VE_SPHERE: obj = (void*)v->base->spheres[obj_idx]; break;
            case VE_MESH: obj = (void*)v->base->meshes[obj_idx]; break;
            default: obj = 0; break;
            }

            v->events = realloc(v->events, sizeof(struct VideoEvent*) * ++v->nevents);
            v->events[v->nevents - 1] = ve_alloc(vtype, obj, delta, fbegin, fcount);
        }
    }
}


void *video_parse_obj(struct Video *v, char *line, int type)
{
    char *s = line;
    while (*s != '>') ++s;
    ++s;

    switch (type)
    {
    case VE_SPHERE: return (void*)scene_parse_sphere(s, v->base->mats);
    case VE_MESH: return (void*)scene_parse_mesh(s, v->base->mats);
    }

    return 0;
}


void video_create(struct Video *v)
{
    for (size_t i = 0; i < v->nframes; ++i)
    {
        printf("== Frame %zu (%.2f%%) ==\n", i + 1, ((float)(i + 1) / (float)v->nframes) * 100.f);

        for (size_t j = 0; j < v->nevents; ++j)
        {
            struct VideoEvent *evt = v->events[j];

            if (i >= evt->fbegin && i <= evt->fbegin + evt->fcount)
                video_apply_delta(evt->obj, evt->delta, evt->type);
        }

        render_rend(v->base);
        char cmd[100] = { 0 };
        sprintf(cmd, "ffmpeg -i out.ppm frames/%zu.png &> /dev/null", i);
        system(cmd);
    }

    system("ffmpeg -i frames/%d.png out.mp4 &> /dev/null");
}


void video_apply_delta(void *obj, void *delta, int type)
{
    switch (type)
    {
    case VE_SPHERE:
    {
        struct Sphere *sphere = (struct Sphere*)obj;
        struct Sphere *d = (struct Sphere*)delta;

        sphere->c = vec_addv(sphere->c, d->c);
        sphere->r += d->r;
    } break;
    case VE_MESH:
    {
        struct Mesh *m = (struct Mesh*)obj;
        struct Mesh *d = (struct Mesh*)delta;

        m->pos = vec_addv(m->pos, d->pos);
        mesh_find_bounds(m, (Vec3f){ 0.f, 0.f, 0.f });
    } break;
    }
}

