#include "scene.h"
#include "render.h"
#include "util.h"
#include <limits.h>


struct Scene *scene_alloc(const char *fp)
{
    struct Scene *s = malloc(sizeof(struct Scene));
    s->spheres = 0;
    s->nspheres = 0;
    s->mats = 0;
    s->nmats = 0;
    s->lights = 0;
    s->nlights = 0;
    s->meshes = 0;
    s->nmeshes = 0;

    FILE *f = fopen(fp, "r");

    char *line = 0;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, f)) != -1)
    {
        if (read == 1)
            continue;

        char word[11] = { 0 };
        sscanf(line, "%10s", word);

        if (strcmp(word, "dim") == 0)
        {
            int x, y;
            sscanf(line, "%*s %d %d", &x, &y);
            render_set_dim(x, y);
        }
        else if (strcmp(word, "sphere") == 0)
        {
            char *new = line;
            while (*new != ' ') ++new;
            ++new;

            s->spheres = realloc(s->spheres, sizeof(struct Sphere*) * ++s->nspheres);
            s->spheres[s->nspheres - 1] = scene_parse_sphere(new, s->mats);
        }
        else if (strcmp(word, "light") == 0)
        {
            char *new = line;
            while (*new != ' ') ++new;
            ++new;

            s->lights = realloc(s->lights, sizeof(Light) * ++s->nlights);
            s->lights[s->nlights - 1] = scene_parse_light(new);
        }
        else if (strcmp(word, "mesh") == 0)
        {
            char *new = line;
            while (*new != ' ') ++new;
            ++new;

            s->meshes = realloc(s->meshes, sizeof(struct Mesh*) * ++s->nmeshes);
            s->meshes[s->nmeshes - 1] = scene_parse_mesh(new, s->mats);
        }
        else if (strcmp(word, "material") == 0)
        {
            Material mat;
            sscanf(line, "%*s %f %f %f|%f %f %f|%f", &mat.col.x, &mat.col.y, &mat.col.z,
                    &mat.ref_diffuse, &mat.ref_specular, &mat.ref_mirror, &mat.specular_exp);

            s->mats = realloc(s->mats, sizeof(Material) * ++s->nmats);
            s->mats[s->nmats - 1] = mat;
        }
        else if (strcmp(word, "threads") == 0)
        {
            int threads;
            sscanf(line, "%*s %d", &threads);
            render_set_threads(threads);
        }
        else if (strcmp(word, "bg") == 0)
        {
            Vec3f bg;
            sscanf(line, "%*s %f %f %f", &bg.x, &bg.y, &bg.z);
            render_set_bg(bg);
        }
        else if (strcmp(word, "antialias") == 0)
        {
            render_enable_antialiasing();
        }
        else if (strcmp(word, "optimize") == 0)
        {
            char next[11] = { 0 };
            sscanf(line, "%*s %10s", next);

            if (strcmp(next, "backface") == 0) render_enable_optimizations(OPT_BACKFACE_CULLING);
            else
            {
                printf("** WARNING **: Unrecognized optimization '%s'; skipping.\n", next);
            }
        }
        else if (strcmp(word, "bounces") == 0)
        {
            int b;
            sscanf(line, "%*s %d", &b);
            render_set_max_bounces(b);
        }
        else if (strncmp(word, "//", 2) == 0)
        {
            continue;
        }
        else if (strcmp(word, "image") == 0)
        {
            continue;
        }
        else
        {
            printf("** WARNING **: Unrecognized directive '%s'; skipping.\n", word);
        }
    }

    free(line);
    fclose(f);

    return s;
}


void scene_free(struct Scene *s)
{
    for (size_t i = 0; i < s->nspheres; ++i)
        sphere_free(s->spheres[i]);

    for (size_t i = 0; i < s->nmeshes; ++i)
        mesh_free(s->meshes[i]);

    free(s->lights);
    free(s->mats);
    free(s);
}


struct Sphere *scene_parse_sphere(char *s, Material *mats)
{
    Vec3f pos;
    float radius;
    int mat_idx;

    sscanf(s, "%f %f %f|%d|%f\n", &pos.x, &pos.y, &pos.z,
        &mat_idx, &radius);

    return sphere_alloc(pos, radius, mats[mat_idx]);
}


struct Mesh *scene_parse_mesh(char *s, Material *mats)
{
    Vec3f pos;
    char fp[PATH_MAX];
    int fullscreen_bounds;
    int mat_idx;

    sscanf(s, "%f %f %f|%d|%s %d", &pos.x, &pos.y, &pos.z,
            &mat_idx, fp, &fullscreen_bounds);

    struct Mesh *m = mesh_alloc(pos, fp, mats[mat_idx]);

    if (fullscreen_bounds)
    {
        m->left_rx = -1.f;
        m->right_rx = 1.f;
        m->bot_ry = 1.f;
        m->top_ry = -1.f;
    }

    return m;
}


Light scene_parse_light(char *s)
{
    Vec3f pos;
    float in;

    sscanf(s, "%f %f %f|%f", &pos.x, &pos.y, &pos.z, &in);

    Light l = { pos, in };
    return l;
}

