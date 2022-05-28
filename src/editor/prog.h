#ifndef EDITOR_PROG_H
#define EDITOR_PROG_H

#include "toolbar.h"
#include <core/scene.h>
#include <core/raytrace.h>
#include <stdbool.h>
#include <pthread.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

enum { MODE_NORMAL, MODE_OBJECT };

struct Prog
{
    bool running;

    SDL_Window *window;
    SDL_Renderer *rend;

    TTF_Font *font;

    struct Scene *sc;
    struct Config *config;

    int mode;

    bool focused;

    struct Mesh *selected_mesh, *hover_mesh;

    struct Toolbar *toolbar;

    SDL_Texture *status;
    struct timespec last_status;

    bool rendering;
    pthread_t render_thread;
    raytrace_args_t *render_thread_args;
};

struct Prog *prog_alloc(SDL_Window *w, SDL_Renderer *r);
void prog_free(struct Prog *p);

void prog_mainloop(struct Prog *p);
void prog_events(struct Prog *p, SDL_Event *evt);

void prog_sample_button(struct Prog *p);

#endif

