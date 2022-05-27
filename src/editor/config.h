#ifndef EDITOR_CONFIG_H
#define EDITOR_CONFIG_H

#include <core/util.h>
#include <SDL2/SDL.h>

struct Config
{
    SDL_Point dim;
    int threads;
};

struct Config *config_alloc();
void config_free(struct Config *c);

#endif

