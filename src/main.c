#include "raytrace.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
        raytrace_image(argv[1], "out.ppm");
    else if (strcmp(type, "video") == 0)
        raytrace_image(argv[1], "out.mp4");

    return 0;
}
