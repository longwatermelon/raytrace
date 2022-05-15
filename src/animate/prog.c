#include "prog.h"


struct Prog *prog_alloc(SDL_Window *w, SDL_Renderer *r)
{
    struct Prog *p = malloc(sizeof(struct Prog));
    p->running = true;

    p->window = w;
    p->rend = r;

    return p;
}


void prog_free(struct Prog *p)
{
    free(p);
}


void prog_mainloop(struct Prog *p)
{
    SDL_Event evt;

    while (p->running)
    {
        prog_events(p, &evt);

        SDL_RenderClear(p->rend);

        SDL_SetRenderDrawColor(p->rend, 0, 0, 0, 255);
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
}

