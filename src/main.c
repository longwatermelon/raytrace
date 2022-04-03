#include "render.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <linux/limits.h>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        fprintf(stderr, "Error: no configuration file provided.\n");
        exit(EXIT_FAILURE);
    }

    struct Scene *scene = scene_alloc(argv[1]);
    render_set_scene(scene);
    render_rend();
    scene_free(scene);

    return 0;
}
