#ifndef EDITOR_PROG_H
#define EDITOR_PROG_H

#include <core/scene.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

enum { MODE_NORMAL, MODE_OBJECT };

struct Prog
{
    bool running;

    SDL_Window *window;
    SDL_Renderer *rend;

    struct Scene *sc;
    int mode;

    bool focused;

    struct Mesh *selected_mesh;
};

struct Prog *prog_alloc(SDL_Window *w, SDL_Renderer *r);
void prog_free(struct Prog *p);

void prog_mainloop(struct Prog *p);
void prog_events(struct Prog *p, SDL_Event *evt);

void prog_render_toolbar(struct Prog *p);

#endif

