#include "writer.h"

#define EXPAND_VECTOR(v) v.x, v.y, v.z


void writer_write_image(struct Scene *sc, const char *fp)
{
    char *out = calloc(sizeof(char), 1);

    const char *begin = "image\n"
                        "dim 1000 1000\n"
                        "threads 4\n"
                        "antialias\n"
                        "optimize backface\n"
                        "bg %.2f %.2f %.2f\n"
                        "cam %.2f %.2f %.2f|%.2f %.2f %.2f\n";

    out = realloc(out, sizeof(char) * (strlen(begin) + 100));
    sprintf(out, begin, EXPAND_VECTOR(sc->bg), EXPAND_VECTOR(sc->cam->pos), EXPAND_VECTOR(sc->cam->angle));
    out = realloc(out, sizeof(char) * (strlen(out) + 1));

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

        sprintf(s, template, m->col.x, m->col.y, m->col.z,
            m->ref_diffuse, m->ref_specular, m->ref_mirror,
            m->specular_exp, idx);

        out = realloc(out, sizeof(char) * (strlen(out) + strlen(s) + 1));
        strcat(out, s);
        free(s);
    }

    FILE *f = fopen(fp, "w");
    fputs(out, f);
    fclose(f);

    free(out);
#if 0
image
dim 1000 1000
threads 4
antialias
optimize backface
bg .1 0 .1
//cam -10 -2 0|.5 -.2 0
cam 0 0 0|0 0 0

// meshes
material .6 .95 .6|.9 .1 1|10|-1
material .7 .95 1|1 .6 .4|70|-1
material .9 .6 .7|1 2 1|50|-1
material .6 .7 .9|.9 .05 1|10|-1
material .95 .7 .95|1 1 1|50|-1

// spheres
material .8 .8 .9|1 1 .8|70|-1
material .9 .7 .7|1 1 .6|70|-1

sphere -3 3 14|5|1
sphere 2 2 25|6|2

//mesh 0 4 0|0|res/floor.obj 1
mesh -3.8 3 20|1|res/cube.obj 0
mesh 2 3 17|2|res/icosphere.obj 0
//mesh -1 2.2 23|3|res/donut.obj 0
mesh -1 3.4 12|4|res/monkey.obj 0

light 4 -1 5|.8
light -3 -5 11|.8
#endif
}

