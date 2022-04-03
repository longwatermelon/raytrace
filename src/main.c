#include "render.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <linux/limits.h>

struct Scene *configure(const char *fp)
{
    FILE *f = fopen(fp, "r");

    if (!f)
    {
        fprintf(stderr, "Error: file '%s' does not exist.\n", fp);
        exit(EXIT_FAILURE);
    }

    char *line = 0;
    size_t len = 0;
    ssize_t read;

    struct Sphere **spheres = 0;
    size_t nspheres = 0;

    struct Mesh **meshes = 0;
    size_t nmeshes = 0;

    Light *lights = 0;
    size_t nlights = 0;

    Material *materials = 0;
    size_t nmaterials = 0;

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
            Vec3f pos;
            float radius;
            int mat_idx;

            sscanf(line, "%*s %f %f %f|%d|%f\n", &pos.x, &pos.y, &pos.z,
                    &mat_idx, &radius);

            spheres = realloc(spheres, sizeof(struct Sphere*) * ++nspheres);
            spheres[nspheres - 1] = sphere_alloc(pos, radius, materials[mat_idx]);
        }
        else if (strcmp(word, "light") == 0)
        {
            Vec3f pos;
            float in;

            sscanf(line, "%*s %f %f %f|%f", &pos.x, &pos.y, &pos.z, &in);

            lights = realloc(lights, sizeof(Light) * ++nlights);
            lights[nlights - 1] = (Light){ pos, in };
        }
        else if (strcmp(word, "mesh") == 0)
        {
            Vec3f pos;
            char fp[PATH_MAX];
            int fullscreen_bounds;
            int mat_idx;

            sscanf(line, "%*s %f %f %f|%d|%s %d", &pos.x, &pos.y, &pos.z,
                    &mat_idx, fp, &fullscreen_bounds);

            meshes = realloc(meshes, sizeof(struct Mesh*) * ++nmeshes);
            meshes[nmeshes - 1] = mesh_alloc(pos, fp, materials[mat_idx]);

            if (fullscreen_bounds)
            {
                struct Mesh *m = meshes[nmeshes - 1];

                m->left_rx = -1.f;
                m->right_rx = 1.f;
                m->bot_ry = 1.f;
                m->top_ry = -1.f;
            }
        }
        else if (strcmp(word, "material") == 0)
        {
            Material mat;
            sscanf(line, "%*s %f %f %f|%f %f %f|%f", &mat.col.x, &mat.col.y, &mat.col.z,
                    &mat.ref_diffuse, &mat.ref_specular, &mat.ref_mirror, &mat.specular_exp);

            materials = realloc(materials, sizeof(Material) * ++nmaterials);
            materials[nmaterials - 1] = mat;
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
        else
        {
            printf("** WARNING **: Unrecognized directive '%s'; skipping.\n", word);
        }
    }

    free(line);
    fclose(f);

    return scene_alloc(
        spheres, nspheres,
        meshes, nmeshes,
        lights, nlights
    );
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        fprintf(stderr, "Error: no configuration file provided.\n");
        exit(EXIT_FAILURE);
    }

    struct Scene *scene = configure(argv[1]);
    render_set_scene(scene);
    render_rend();
    scene_free(scene);

    return 0;
}
