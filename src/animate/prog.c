#include "prog.h"
#include "core/rasterize.h"
#include "render.h"
#include <SDL2/SDL_mouse.h>
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
    p->focused = false;
    p->selected_mesh = 0;

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
        SDL_Point center = { w / 2, h / 2 };

        prog_events(p, &evt);

        if (p->focused)
        {
            SDL_Point mouse;
            SDL_GetMouseState(&mouse.x, &mouse.y);

            mouse.x -= center.x;
            mouse.y -= center.y;

            SDL_WarpMouseInWindow(p->window, center.x, center.y);

            p->sc->cam->angle.x += (float)mouse.x / 400.f;
            p->sc->cam->angle.y -= (float)mouse.y / 400.f;
        }

        Vec3f dir = rasterize_rotate_cc((Vec3f){ 0.f, 0.f, 1.f }, p->sc->cam->angle);
        if (!render_scene_cast_ray(p->sc, p->sc->cam->pos, dir, true, 0, 0, (void*)&p->selected_mesh, 0))
            p->selected_mesh = 0;

        SDL_RenderClear(p->rend);

        render_scene(p->sc, p->selected_mesh, p->rend);

        SDL_SetRenderDrawColor(p->rend, 255, 255, 255, 255);
        SDL_RenderDrawLine(p->rend, center.x, center.y - 10, center.x, center.y + 10);
        SDL_RenderDrawLine(p->rend, center.x - 10, center.y, center.x + 10, center.y);

        SDL_SetRenderDrawColor(p->rend, p->sc->bg.x * 255.f, p->sc->bg.y * 255.f, p->sc->bg.z * 255.f, 255);
        SDL_RenderPresent(p->rend);
    }
}


void prog_events(struct Prog *p, SDL_Event *evt)
{
    struct Camera *c = p->sc->cam;

    while (SDL_PollEvent(evt))
    {
        switch (evt->type)
        {
        case SDL_QUIT:
            p->running = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
        {
            p->focused = true;
            SDL_ShowCursor(SDL_FALSE);
        } break;
        case SDL_KEYDOWN:
        {
            switch (evt->key.keysym.sym)
            {
            case SDLK_ESCAPE:
                p->focused = false;
                SDL_ShowCursor(SDL_TRUE);
                break;
            }
        } break;
        }
    }

    if (p->focused)
    {
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
    }
}

