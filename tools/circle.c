#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: circle [interval] [radius]\n");
        exit(EXIT_FAILURE);
    }

    float interval = atof(argv[1]);
    float r = atof(argv[2]);
    float angle = 0.f;

    float x = 0.f;
    float y = 0.f;

    for (int i = 0; i < 4.f * M_PI / interval; ++i)
    {
        x = r * cosf(angle) - x;
        y = r * sinf(angle) - y;

        const char *template = "evt 0|%d %d|0>%f 0 %f|0|0\n";
        printf(template, i, i, x, y);

        x = r * cosf(angle);
        y = r * sinf(angle);

        angle += interval;
    }
}

