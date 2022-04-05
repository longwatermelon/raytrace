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

    FILE *fp = fopen(argv[1], "r");

    if (!fp)
    {
        fprintf(stderr, "ERROR: '%s' does not exist.\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    char type[11] = { 0 };
    fscanf(fp, "%s", type);
    fclose(fp);

    if (strcmp(type, "image") == 0)
    {
        struct Scene *scene = scene_alloc(argv[1]);
        render_rend(scene);
        scene_free(scene);
    }
    else if (strcmp(type, "video") == 0)
    {
        struct Video *v = video_alloc(argv[1]);
        video_create(v);
        video_free(v);
    }

    return 0;
}
