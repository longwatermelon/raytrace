#include "render.h"
#include "video.h"
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

    if (argc >= 3 && strcmp(argv[2], "video") == 0)
    {
        struct Video *v = video_alloc(argv[1]);
        video_create(v);
        video_free(v);
    }
    else
    {
        struct Scene *scene = scene_alloc(argv[1]);
        render_set_scene(scene);
        render_rend();
        scene_free(scene);
    }

    return 0;
}
