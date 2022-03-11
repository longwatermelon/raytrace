#include "render.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <linux/limits.h>

void configure(const char *fp)
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

    while ((read = getline(&line, &len, f)) != -1)
    {
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
            Vec3f pos, col;
            float radius;

            sscanf(line, "%*s %f %f %f|%f %f %f|%f\n", &pos.x, &pos.y, &pos.z,
                    &col.x, &col.y, &col.z, &radius);

            spheres = realloc(spheres, sizeof(struct Sphere*) * ++nspheres);
            spheres[nspheres - 1] = sphere_alloc(pos, radius, col);
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
            Vec3f pos, col;
            char fp[PATH_MAX];
            int fullscreen_bounds;

            sscanf(line, "%*s %f %f %f|%f %f %f|%s %d", &pos.x, &pos.y, &pos.z,
                    &col.x, &col.y, &col.z, fp, &fullscreen_bounds);

            meshes = realloc(meshes, sizeof(struct Mesh*) * ++nmeshes);
            meshes[nmeshes - 1] = mesh_alloc(pos, fp, col);

            if (fullscreen_bounds)
            {
                struct Mesh *m = meshes[nmeshes - 1];

                m->left_rx = -1.f;
                m->right_rx = 1.f;
                m->bot_ry = 1.f;
                m->top_ry = -1.f;
            }
        }
    }

    render_set_spheres(spheres, nspheres);
    render_set_lights(lights, nlights);
    render_set_meshes(meshes, nmeshes);

    free(line);
    fclose(f);
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        fprintf(stderr, "Error: no configuration file provided.\n");
        exit(EXIT_FAILURE);
    }

    configure(argv[1]);

    // struct Sphere *spheres[] = {
    //     sphere_alloc((Vec3f){ -3.f, 3.f, 14.f }, 1.f, (Vec3f){ 1.f, .95f, .7f }),
    //     sphere_alloc((Vec3f){ 2.f, 2.f, 25.f }, 2.f, (Vec3f){ 1.f, .5f, 1.f })
    // };

    // size_t nspheres = sizeof(spheres) / sizeof(struct Sphere*);

    // Light lights[] = {
    //     (Light){ (Vec3f){ 4.f, -1.f, 5.f }, 0.8f },
    //     (Light){ (Vec3f){ -3.f, -5.f, 11.f }, 0.8f }
    // };

    // size_t nlights = sizeof(lights) / sizeof(Light);

    // struct Mesh *meshes[] = {
    //     mesh_alloc((Vec3f){ 0.f, 4.f, 0.f }, "res/floor.obj", (Vec3f){ .5f, .9f, .5f }),
    //     mesh_alloc((Vec3f){ -3.f, 3.f, 20.f }, "res/cube.obj", (Vec3f){ .7f, .95f, 1.f }),
    //     mesh_alloc((Vec3f){ 2.f, 3.f, 17.f }, "res/icosphere.obj", (Vec3f){ .9f, .6f, .7f }),
    //     mesh_alloc((Vec3f){ -1.f, 2.2f, 23.f }, "res/donut.obj", (Vec3f){ .6f, .7f, .9f }),
    //     mesh_alloc((Vec3f){ -1.f, 3.4f, 12.f }, "res/monkey.obj", (Vec3f){ .95f, .7f, .95f }),
    //     mesh_alloc((Vec3f){ 3.2f, 3.f, 13.f }, "res/cylinder.obj", (Vec3f){ .95f, .7f, .7f }),
    // };

    // size_t nmeshes = sizeof(meshes) / sizeof(struct Mesh*);

    // render_set_dim(1000, 1000);
    // render_set_spheres(spheres, nspheres);
    // render_set_lights(lights, nlights);
    // render_set_meshes(meshes, nmeshes);

    render_enable_antialiasing();
    render_set_threads(8);

    render_rend();

    // for (size_t i = 0; i < nspheres; ++i)
    //     sphere_free(spheres[i]);

    // for (size_t i = 0; i < nmeshes; ++i)
    //     mesh_free(meshes[i]);

    return 0;
}
