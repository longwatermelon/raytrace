#include "toolbar.h"
#include "util.h"
#include "prog.h"


struct Toolbar *toolbar_alloc(SDL_Window *w, SDL_Renderer *rend, TTF_Font *font)
{
    struct Toolbar *t = malloc(sizeof(struct Toolbar));
    t->win = w;
    SDL_Point ssize = util_ssize(w);

    for (int i = 0; i < 3; ++i)
        t->obj_pos[i] = slider_alloc((SDL_Point){ ssize.x + 10, 50 + i * 30 }, .01f, 0.f, rend, font);

    return t;
}


void toolbar_free(struct Toolbar *t)
{
    for (int i = 0; i < 3; ++i)
        slider_free(t->obj_pos[i]);

    free(t);
}


void toolbar_render(struct Toolbar *t, SDL_Renderer *rend)
{
    SDL_Point ssize = util_ssize(t->win);
    SDL_SetRenderDrawColor(rend, 40, 40, 40, 255);
    SDL_Rect r = { ssize.x, 0, EDITOR_TOOLBAR_WIDTH, ssize.y };
    SDL_RenderFillRect(rend, &r);

    for (int i = 0; i < 3; ++i)
        slider_render(t->obj_pos[i], rend);
}


void toolbar_main(struct Toolbar *t, struct Prog *p)
{
    toolbar_update_positions(t);

    if (p->focused)
    {
        if (p->selected_mesh)
        {
            t->obj_pos[0]->value = p->selected_mesh->pos.x;
            t->obj_pos[1]->value = p->selected_mesh->pos.y;
            t->obj_pos[2]->value = p->selected_mesh->pos.z;
        }
    }

    for (int i = 0; i < 3; ++i)
        slider_redo_tex(t->obj_pos[i], p->rend, p->font);
}


void toolbar_update_positions(struct Toolbar *t)
{
    SDL_Point ssize = util_ssize(t->win);

    for (int i = 0; i < 3; ++i)
        t->obj_pos[i]->rect.x = ssize.x + 10;
}

