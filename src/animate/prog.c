#include "prog.h"
#include "render.h"
#include <core/render.h>
#include <core/scene.h>
#include <SDL2/SDL.h>


struct Prog *prog_alloc(SDL_Window *w, SDL_Renderer *r)
{
    struct Prog *p = malloc(sizeof(struct Prog));
    p->running = true;

    p->window = w;
    p->rend = r;

    p->sc = scene_alloc("examples/image");

    return p;
}


void prog_free(struct Prog *p)
{
    if (p->sc)
        scene_free(p->sc);

    free(p);
}


void prog_mainloop(struct Prog *p)
{
    SDL_Event evt;

    while (p->running)
    {
        int w, h;
        SDL_GetWindowSize(p->window, &w, &h);
        render_set_size(w, h);

        prog_events(p, &evt);

        SDL_RenderClear(p->rend);

        render_scene(p->sc, p->rend);

        SDL_SetRenderDrawColor(p->rend, p->sc->bg.x * 255.f, p->sc->bg.y * 255.f, p->sc->bg.z * 255.f, 255);
        SDL_RenderPresent(p->rend);
    }
}


void prog_events(struct Prog *p, SDL_Event *evt)
{
    while (SDL_PollEvent(evt))
    {
        switch (evt->type)
        {
        case SDL_QUIT:
            p->running = false;
            break;
        }
    }

    struct Camera *c = p->sc->cam;
    const Uint8 *keys = SDL_GetKeyboardState(0);

    if (keys[SDL_SCANCODE_W])
    {
        c->pos.z += .1f * cosf(c->angle.x);
        c->pos.x += .1f * sinf(c->angle.x);
    }

    if (keys[SDL_SCANCODE_S])
    {
        c->pos.z -= .1f * cosf(c->angle.x);
        c->pos.x -= .1f * sinf(c->angle.x);
    }

    if (keys[SDL_SCANCODE_A])
    {
        c->pos.x += .1f * sinf(-M_PI / 2.f + c->angle.x);
        c->pos.z += .1f * cosf(-M_PI / 2.f + c->angle.x);
    }

    if (keys[SDL_SCANCODE_D])
    {
        c->pos.x -= .1f * sinf(-M_PI / 2.f + c->angle.x);
        c->pos.z -= .1f * cosf(-M_PI / 2.f + c->angle.x);
    }

    if (keys[SDL_SCANCODE_SPACE]) c->pos.y -= .1f;
    if (keys[SDL_SCANCODE_LSHIFT]) c->pos.y += .1f;

    if (keys[SDL_SCANCODE_UP]) c->angle.y += .05f;
    if (keys[SDL_SCANCODE_DOWN]) c->angle.y -= .05f;
    if (keys[SDL_SCANCODE_RIGHT]) c->angle.x += .05f;
    if (keys[SDL_SCANCODE_LEFT]) c->angle.x -= .05f;
}

