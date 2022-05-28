#include "toolbar.h"
#include "util.h"
#include "prog.h"
#include "config.h"

// button functions
void decrease_threads(struct Prog *p)
{
    if (p->config->threads > 1)
        --p->config->threads;

    char s[20] = { 0 };
    sprintf(s, "%d", p->config->threads);

    SDL_DestroyTexture(p->toolbar->threads_num);
    p->toolbar->threads_num = util_render_text(p->rend, p->font, s, (SDL_Color){ 255, 255, 255 });
}

void increase_threads(struct Prog *p)
{
    ++p->config->threads;
    char s[20] = { 0 };
    sprintf(s, "%d", p->config->threads);

    SDL_DestroyTexture(p->toolbar->threads_num);
    p->toolbar->threads_num = util_render_text(p->rend, p->font, s, (SDL_Color){ 255, 255, 255 });
}

struct Toolbar *toolbar_alloc(struct Prog *p)
{
    struct Toolbar *t = malloc(sizeof(struct Toolbar));
    t->p = p;

    strcpy(t->obj, "None");
    t->obj_tex = util_render_text(p->rend, p->font, "None", (SDL_Color){ 255, 255, 255 });

    SDL_Point ssize = util_ssize(t->p->window);

    char *labels[6] = { "x: ", "y: ", "z: ", "yaw: ", "pitch: ", "roll: " };
    for (int i = 0; i < 3; ++i)
        t->obj_props[i] = slider_alloc((SDL_Point){ ssize.x + 10, 50 + i * 30 }, .01f, 0.f, labels[i], t->p->rend, t->p->font);

    for (int i = 3; i < 6; ++i)
        t->obj_props[i] = slider_alloc((SDL_Point){ ssize.x + 10 + 100 + 20, 50 + (i - 3) * 30 }, .01f, 0.f, labels[i], t->p->rend, t->p->font);

    t->selected_slider = 0;
    t->pressed_button = 0;

    t->nbuttons = 2;
    t->buttons = malloc(sizeof(struct Button*) * t->nbuttons);

    t->threads_text = util_render_text(p->rend, p->font, "Threads", (SDL_Color){ 255, 255, 255 });
    t->threads_num = util_render_text(p->rend, p->font, "4", (SDL_Color){ 255, 255, 255 });
    t->buttons[0] = button_alloc((SDL_Rect){ 10, 200, 20, 20 }, decrease_threads, "-", p->rend, p->font);
    t->buttons[1] = button_alloc((SDL_Rect){ 90, 200, 20, 20 }, increase_threads, "+", p->rend, p->font);

    return t;
}


void toolbar_free(struct Toolbar *t)
{
    for (int i = 0; i < 6; ++i)
        slider_free(t->obj_props[i]);

    if (t->obj_tex)
        SDL_DestroyTexture(t->obj_tex);

    for (size_t i = 0; i < t->nbuttons; ++i)
        button_free(t->buttons[i]);

    free(t->buttons);

    SDL_DestroyTexture(t->threads_text);
    SDL_DestroyTexture(t->threads_num);

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

    for (int i = 0; i < 6; ++i)
        slider_render(t->obj_props[i], t->p->rend);

    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);

    for (size_t i = 0; i < t->nbuttons; ++i)
    {
        t->buttons[i]->rect.x += ssize.x;
        button_render(t->buttons[i], t->p->rend, mouse);
        t->buttons[i]->rect.x -= ssize.x;
    }

    r = (SDL_Rect){ ssize.x + 10, 170 };
    SDL_QueryTexture(t->threads_text, 0, 0, &r.w, &r.h);
    SDL_RenderCopy(t->p->rend, t->threads_text, 0, &r);

    SDL_QueryTexture(t->threads_num, 0, 0, &r.w, &r.h);
    r.x = ssize.x + 55;
    r.y = 202;
    SDL_RenderCopy(t->p->rend, t->threads_num, 0, &r);
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

    struct Slider **arr = t->obj_props;
    struct Mesh *m = t->p->selected_mesh;

    if (t->p->focused)
    {
        if (t->p->selected_mesh)
        {
            arr[0]->value = m->pos.x;
            arr[1]->value = m->pos.y;
            arr[2]->value = m->pos.z;

            arr[3]->value = m->rot.x;
            arr[4]->value = m->rot.y;
            arr[5]->value = m->rot.z;
        }
    }
    else
    {
        if (t->p->selected_mesh)
        {
            m->pos = (Vec3f){
                arr[0]->value,
                arr[1]->value,
                arr[2]->value
            };

            m->rot = (Vec3f){
                arr[3]->value,
                arr[4]->value,
                arr[5]->value
            };
        }
    }

    for (int i = 0; i < 6; ++i)
        slider_redo_tex(t->obj_props[i], t->p->rend, t->p->font);
}


void toolbar_update_positions(struct Toolbar *t)
{
    SDL_Point ssize = util_ssize(t->p->window);

    for (int i = 0; i < 3; ++i)
        t->obj_props[i]->rect.x = ssize.x + 10;

    for (int i = 3; i < 6; ++i)
        t->obj_props[i]->rect.x = ssize.x + 10 + 120;
}


bool toolbar_slide_sliders(struct Toolbar *t, int pixels, bool ignore_accuracy)
{
    bool ret = false;

    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);

    if (t->p->selected_mesh)
    {
        if (t->p->selected_mesh)
        {
            for (int i = 0; i < 6; ++i)
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
                    if (util_point_in_rect(mouse, t->obj_props[i]->rect))
                    {
                        slider_slide(t->obj_props[i], pixels, t->p->rend, t->p->font);
                        t->selected_slider = t->obj_props[i];
                        ret = true;
                    }
                }
            }
        }
    }

    return ret;
}


void toolbar_buttons_pressed(struct Toolbar *t, SDL_Point mouse)
{
    if (t->pressed_button)
        return;

    SDL_Point ssize = util_ssize(t->p->window);

    for (size_t i = 0; i < t->nbuttons; ++i)
    {
        t->buttons[i]->rect.x += ssize.x;

        if (util_point_in_rect(mouse, t->buttons[i]->rect))
        {
            t->pressed_button = t->buttons[i];
            t->buttons[i]->pushed = true;
            t->buttons[i]->callback(t->p);
        }

        t->buttons[i]->rect.x -= ssize.x;
    }
}

