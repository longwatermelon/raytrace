#include "config.h"


struct Config *config_alloc()
{
    struct Config *c = malloc(sizeof(struct Config));
    c->dim = (SDL_Point){ 1000, 1000 };
    c->threads = 4;

    return c;
}


void config_free(struct Config *c)
{
    free(c);
}

