#include "writer.h"
#include "util.h"

#define EXPAND_VECTOR(v) v.x, v.y, v.z


void writer_image(struct Scene *sc, struct Config *c, const char *fp)
{
    char *out = calloc(sizeof(char), 1);

    const char *begin = "image\n"
                        "dim %d %d\n"
                        "threads %d\n"
                        "antialias\n"
                        "optimize backface\n"
                        "bg %.2f %.2f %.2f\n"
                        "cam %.2f %.2f %.2f|%.2f %.2f %.2f\n";

    out = realloc(out, sizeof(char) * (strlen(begin) + 100));

    sprintf(out, begin, c->dim.x, c->dim.y, c->threads,
        EXPAND_VECTOR(sc->bg), EXPAND_VECTOR(sc->cam->pos),
        EXPAND_VECTOR(sc->cam->angle));

    out = realloc(out, sizeof(char) * (strlen(out) + 1));

    writer_image_mats(sc, &out);
    writer_image_meshes(sc, &out);
    writer_image_spheres(sc, &out);
    writer_image_lights(sc, &out);

    FILE *f = fopen(fp, "w");
    fputs(out, f);
    fclose(f);

    free(out);
}


void writer_image_mats(struct Scene *sc, char **out)
{
    for (size_t i = 0; i < sc->nmats; ++i)
    {
        struct Material *m = sc->mats[i];
        int idx = -1;

        for (size_t j = 0; j < sc->ntexs; ++j)
        {
            if (m->tex == sc->texs[j])
            {
                idx = j;
                break;
            }
        }

        const char *template = "material %.2f %.2f %.2f|%.2f %.2f %.2f|%.2f|%d\n";
        char *s = calloc(sizeof(char), strlen(template) + 100);

        sprintf(s, template, EXPAND_VECTOR(m->col),
            m->ref_diffuse, m->ref_specular, m->ref_mirror,
            m->specular_exp, idx);

        s = realloc(s, sizeof(char) * (strlen(s) + 1));

        util_strcat(out, s);
        free(s);
    }
}


void writer_image_meshes(struct Scene *sc, char **out)
{
    for (size_t i = 0; i < sc->nmeshes; ++i)
    {
        struct Mesh *m = sc->meshes[i];

        int idx = -1;

        for (size_t j = 0; j < sc->nmats; ++j)
        {
            if (m->mat == sc->mats[j])
            {
                idx = j;
                break;
            }
        }

        const char *template = "mesh %.2f %.2f %.2f|%d|%s %d\n";
        char *s = calloc(sizeof(char), strlen(template) + 100);
        sprintf(s, template, EXPAND_VECTOR(m->pos), idx, m->name, !m->bounded);

        s = realloc(s, sizeof(char) * (strlen(s) + 1));

        util_strcat(out, s);
        free(s);
    }
}


void writer_image_spheres(struct Scene *sc, char **out)
{
    for (size_t i = 0; i < sc->nspheres; ++i)
    {
        struct Sphere *s = sc->spheres[i];

        int idx = -1;

        for (size_t j = 0; j < sc->nmats; ++j)
        {
            if (s->mat == sc->mats[j])
            {
                idx = j;
                break;
            }
        }

        const char *template = "sphere %.2f %.2f %.2f|%d|%.2f\n";

        char *str = calloc(sizeof(char), strlen(template) + 100);
        sprintf(str, template, EXPAND_VECTOR(s->c), idx, s->r);

        str = realloc(str, sizeof(char) * (strlen(str) + 1));

        util_strcat(out, str);
        free(str);
    }
}


void writer_image_lights(struct Scene *sc, char **out)
{
    for (size_t i = 0; i < sc->nlights; ++i)
    {
        struct Light *l = sc->lights[i];

        const char *template = "light %.2f %.2f %.2f|%.2f\n";
        char *s = calloc(sizeof(char), strlen(template) + 100);
        sprintf(s, template, EXPAND_VECTOR(l->pos), l->in);

        s = realloc(s, sizeof(char) * (strlen(s) + 1));

        util_strcat(out, s);
        free(s);
    }
}

