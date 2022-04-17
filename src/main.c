#include "raytrace.h"
#include "image.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    struct Image *img = image_alloc("test.png");

    for (int i = 0; i < 10; ++i)
    {
        Vec3f col = image_at(img, i, 0);
        printf("Pixel %d: %f %f %f\n", i, col.x, col.y, col.z);
    }

    image_free(img);

    return 0;
    if (argc == 1)
    {
        fprintf(stderr, "ERROR: no configuration file provided.\n");
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
        raytrace_image(argv[1], "out.ppm");
    else if (strcmp(type, "video") == 0)
        raytrace_video(argv[1], "out.mp4");

    return 0;
}
