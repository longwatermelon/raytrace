#include "toolbar.h"
#include "util.h"
#include "prog.h"


struct Toolbar *toolbar_alloc(struct Prog *p)
{
    struct Toolbar *t = malloc(sizeof(struct Toolbar));
    t->p = p;

    strcpy(t->obj, "None");
    t->obj_tex = util_render_text(p->rend, p->font, "None", (SDL_Color){ 255, 255, 255 });

    SDL_Point ssize = util_ssize(t->p->window);

    for (int i = 0; i < 3; ++i)
        t->obj_pos[i] = slider_alloc((SDL_Point){ ssize.x + 10, 50 + i * 30 }, .01f, 0.f, t->p->rend, t->p->font);

    t->selected_slider = 0;

    return t;
}


void toolbar_free(struct Toolbar *t)
{
    for (int i = 0; i < 3; ++i)
        slider_free(t->obj_pos[i]);

    if (t->obj_tex)
        SDL_DestroyTexture(t->obj_tex);

    free(t);
}


void toolbar_render(struct Toolbar *t)
{
    SDL_Point ssize = util_ssize(t->p->window);
    SDL_SetRenderDrawColor(t->p->rend, 40, 40, 40, 255);
    SDL_Rect r = { ssize.x, 0, EDITOR_TOOLBAR_WIDTH, ssize.y };
    SDL_RenderFillRect(t->p->rend, &r);

    SDL_Rect obj_r = { ssize.x + 10, 10 };
    SDL_QueryTexture(t->obj_tex, 0, 0, &obj_r.w, &obj_r.h);
    SDL_RenderCopy(t->p->rend, t->obj_tex, 0, &obj_r);

    for (int i = 0; i < 3; ++i)
        slider_render(t->obj_pos[i], t->p->rend);
}


void toolbar_main(struct Toolbar *t)
{
    bool render_obj = false;

    if (t->p->selected_mesh)
    {
        if (strcmp(t->obj, t->p->selected_mesh->name) != 0)
        {
            sprintf(t->obj, "%s", t->p->selected_mesh->name);
            render_obj = true;
        }
    }
    else
    {
        if (strcmp(t->obj, "None") != 0)
        {
            strcpy(t->obj, "None");
            render_obj = true;
        }
    }

    if (render_obj)
    {
        if (t->obj_tex)
            SDL_DestroyTexture(t->obj_tex);

        t->obj_tex = util_render_text(t->p->rend, t->p->font, t->obj, (SDL_Color){ 255, 255, 255 });
    }

    toolbar_update_positions(t);

    if (t->p->focused)
    {
        if (t->p->selected_mesh)
        {
            t->obj_pos[0]->value = t->p->selected_mesh->pos.x;
            t->obj_pos[1]->value = t->p->selected_mesh->pos.y;
            t->obj_pos[2]->value = t->p->selected_mesh->pos.z;
        }
    }
    else
    {
        if (t->p->selected_mesh)
        {
            t->p->selected_mesh->pos = (Vec3f){
                t->obj_pos[0]->value,
                t->obj_pos[1]->value,
                t->obj_pos[2]->value
            };
        }
    }

    for (int i = 0; i < 3; ++i)
        slider_redo_tex(t->obj_pos[i], t->p->rend, t->p->font);
}


void toolbar_update_positions(struct Toolbar *t)
{
    SDL_Point ssize = util_ssize(t->p->window);

    for (int i = 0; i < 3; ++i)
        t->obj_pos[i]->rect.x = ssize.x + 10;
}


bool toolbar_slide_sliders(struct Toolbar *t, int pixels, bool ignore_accuracy)
{
    bool ret = false;

    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);

    if (t->p->selected_mesh)
    {
        for (int i = 0; i < 3; ++i)
        {
            if (ignore_accuracy)
            {
                if (t->selected_slider)
                {
                    slider_slide(t->selected_slider, pixels, t->p->rend, t->p->font);
                    ret = true;
                }
            }
            else
            {
                if (util_point_in_rect(mouse, t->obj_pos[i]->rect))
                {
                    slider_slide(t->obj_pos[i], pixels, t->p->rend, t->p->font);
                    t->selected_slider = t->obj_pos[i];
                    ret = true;
                }
            }
        }
    }

    return ret;
}

