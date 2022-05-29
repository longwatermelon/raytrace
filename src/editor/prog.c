#include "prog.h"
#include "core/rasterize.h"
#include "render.h"
#include "util.h"
#include "button.h"
#include "slider.h"
#include "writer.h"
#include <core/util.h>
#include <core/raytrace.h>
#include <core/render.h>
#include <core/scene.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL.h>


struct Prog *prog_alloc(SDL_Window *w, SDL_Renderer *r)
{
    struct Prog *p = malloc(sizeof(struct Prog));
    p->running = true;

    p->window = w;
    p->rend = r;

    p->font = TTF_OpenFont("res/font.ttf", 14);

    p->sc = scene_alloc("examples/image");
    p->config = config_alloc();

    p->mode = MODE_NORMAL;
    p->focused = false;
    p->selected_mesh = 0;
    p->hover_mesh = 0;

    p->toolbar = toolbar_alloc(p);

    p->status = 0;
    clock_gettime(CLOCK_MONOTONIC, &p->last_status);

    p->rendering = false;

    p->render_thread = 0;
    p->render_thread_args = 0;

    p->explorer = 0;

    return p;
}


void prog_free(struct Prog *p)
{
    if (p->sc)
        scene_free(p->sc);

    if (p->explorer)
        explorer_free(p->explorer);

    toolbar_free(p->toolbar);
    config_free(p->config);

    TTF_CloseFont(p->font);

    free(p);
}


void prog_mainloop(struct Prog *p)
{
    SDL_Event evt;

    while (p->running)
    {
        SDL_Point ssize = util_ssize(p->window);
        render_set_size(ssize.x, ssize.y);
        SDL_Point center = { ssize.x / 2, ssize.y / 2 };

        prog_events(p, &evt);

        SDL_Point mouse;
        SDL_GetMouseState(&mouse.x, &mouse.y);

        if (p->focused)
        {
            mouse.x -= center.x;
            mouse.y -= center.y;

            SDL_WarpMouseInWindow(p->window, center.x, center.y);

            p->sc->cam->angle.x += (float)mouse.x / 200.f;
            p->sc->cam->angle.y -= (float)mouse.y / 200.f;
        }

        if (p->focused)
        {
            Vec3f dir = rasterize_rotate_cc((Vec3f){ 0.f, 0.f, 1.f }, p->sc->cam->angle);
            if (!render_scene_cast_ray(p->sc, p->sc->cam->pos, dir, (Point){ 0, 0 }, false, 0, 0, (void*)&p->hover_mesh, 0))
                p->hover_mesh = 0;
        }

        toolbar_main(p->toolbar);

        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);

        if (p->status && util_timediff(&p->last_status, &now) >= 1.f)
        {
            if (p->rendering)
            {
                const char *template = "Rendering... [%.2f%%]";
                char s[100];
                float progress = p->sc->progress * 100.f;
                sprintf(s, template, progress);

                SDL_DestroyTexture(p->status);

                if (progress >= 100.f)
                {
                    if (!p->render_thread_args->done)
                    {
                        p->status = util_render_text(p->rend, p->font, "Finishing up...", (SDL_Color){ 255, 255, 0 });
                    }
                    else
                    {
                        p->rendering = false;
                        p->status = util_render_text(p->rend, p->font, "[✔] Done rendering", (SDL_Color){ 0, 255, 0 });
                        clock_gettime(CLOCK_MONOTONIC, &p->last_status);

                        free(p->render_thread_args);

                        p->render_thread = 0;
                        p->render_thread_args = 0;
                    }
                }
                else
                {
                    p->status = util_render_text(p->rend, p->font, s, (SDL_Color){ 255, 255, 0 });
                }
            }
            else
            {
                SDL_DestroyTexture(p->status);
                p->status = 0;
            }
        }

        SDL_RenderClear(p->rend);

        render_scene(p);

        SDL_SetRenderDrawColor(p->rend, 255, 255, 255, 255);
        SDL_RenderDrawLine(p->rend, center.x, center.y - 10, center.x, center.y + 10);
        SDL_RenderDrawLine(p->rend, center.x - 10, center.y, center.x + 10, center.y);

        toolbar_render(p->toolbar);

        if (p->status)
        {
            SDL_Rect r = { 10, 20 };
            SDL_QueryTexture(p->status, 0, 0, &r.w, &r.h);
            SDL_RenderCopy(p->rend, p->status, 0, &r);
        }

        SDL_SetRenderDrawColor(p->rend, p->sc->bg.x * 255.f, p->sc->bg.y * 255.f, p->sc->bg.z * 255.f, 255);
        SDL_RenderPresent(p->rend);
    }
}


void prog_events(struct Prog *p, SDL_Event *evt)
{
    struct Camera *c = p->sc->cam;
    SDL_Point ssize = util_ssize(p->window);

    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);

    static SDL_Point slider_mouse = { -1, -1 };

    static bool mouse_down = false;
    static bool ctrl = false;

    while (SDL_PollEvent(evt))
    {
        if (evt->type == SDL_WINDOWEVENT && evt->window.event == SDL_WINDOWEVENT_CLOSE)
        {
            p->running = false;
            break;
        }

        switch (evt->type)
        {
        case SDL_MOUSEBUTTONDOWN:
        {
            mouse_down = true;

            if (p->focused)
            {
                p->selected_mesh = p->hover_mesh;
            }

            if (!p->focused && evt->button.x < ssize.x && evt->button.y < ssize.y)
            {
                p->focused = true;
                SDL_ShowCursor(SDL_FALSE);
            }

            toolbar_buttons_pressed(p->toolbar, mouse);
        } break;
        case SDL_MOUSEBUTTONUP:
        {
            mouse_down = false;

            if (p->toolbar->selected_slider)
            {
                SDL_SetRelativeMouseMode(SDL_FALSE);
                SDL_WarpMouseInWindow(p->window, slider_mouse.x, slider_mouse.y);
                slider_mouse = (SDL_Point){ -1, -1 };
                p->toolbar->selected_slider = 0;
            }

            if (p->toolbar->pressed_button)
            {
                p->toolbar->pressed_button->pushed = false;
                p->toolbar->pressed_button = 0;
            }
        } break;
        case SDL_MOUSEMOTION:
        {
            if (mouse_down)
            {
                if (toolbar_slide_sliders(p->toolbar, evt->motion.xrel))
                {
                    if (slider_mouse.x == -1 && slider_mouse.y == -1)
                        SDL_GetMouseState(&slider_mouse.x, &slider_mouse.y);

                    SDL_SetHintWithPriority(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1", SDL_HINT_OVERRIDE);
                    SDL_SetRelativeMouseMode(SDL_TRUE);
                }
            }

            p->sc->cam->angle = (Vec3f){
                util_restrict_angle(p->sc->cam->angle.x),
                util_restrict_angle(p->sc->cam->angle.y),
                util_restrict_angle(p->sc->cam->angle.z)
            };
        } break;

        case SDL_KEYDOWN:
        {
            switch (evt->key.keysym.sym)
            {
            case SDLK_ESCAPE:
                p->focused = false;
                SDL_ShowCursor(SDL_TRUE);
                break;
            case SDLK_LCTRL:
            case SDLK_RCTRL:
                ctrl = true;
                break;
            case SDLK_r:
            {
                if (ctrl)
                {
                    p->sc->progress = 0.f;
                    writer_image(p->sc, p->config, ".rtmp");
                    p->status = util_render_text(p->rend, p->font, "Rendering...", (SDL_Color){ 255, 255, 0 });
                    p->rendering = true;

                    raytrace_sc_args_t *args = malloc(sizeof(raytrace_sc_args_t));
                    args->out = "out.ppm";
                    args->sc = p->sc;
                    args->done = false;
                    p->render_thread_args = args;
                    pthread_create(&p->render_thread, 0, raytrace_thr_sc_image, (void*)args);
                    pthread_detach(p->render_thread);
                }
                else
                {
                    writer_image(p->sc, p->config, "out");
                    p->status = util_render_text(p->rend, p->font, "[✔] Saved scene", (SDL_Color){ 0, 255, 0 });
                    clock_gettime(CLOCK_MONOTONIC, &p->last_status);
                }
            } break;
            case SDLK_f:
            {
                p->explorer = explorer_alloc(".", p);
                char *path = explorer_find(p->explorer);

                if (path)
                {
                    p->sc->meshes = realloc(p->sc->meshes, sizeof(struct Mesh*) * ++p->sc->nmeshes);
                    p->sc->meshes[p->sc->nmeshes - 1] = mesh_alloc((Vec3f){ 0.f, 0.f, 0.f }, (Vec3f){ 0.f, 0.f, 0.f }, path, p->sc->mats[0]);
                }

                explorer_free(p->explorer);
                p->explorer = 0;
            } break;
            }
        } break;

        case SDL_KEYUP:
        {
            switch (evt->key.keysym.sym)
            {
            case SDLK_LCTRL:
            case SDLK_RCTRL:
                ctrl = false;
                break;
            }
        } break;
        }
    }

    if (p->focused && p->mode == MODE_NORMAL)
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

