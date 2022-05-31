#include "toolbar.h"
#include "util.h"
#include "prog.h"
#include "config.h"
#include <core/rasterize.h>
#include <core/render.h>

#define CLAMP(x, min, max) fmin(max, fmax(x, min))

void modify_mat_idx(struct Prog *p, int num)
{
    if (p->selected_mesh)
    {
        int idx = scene_mat_idx(p->sc, p->selected_mesh->mat);

        if (idx + num >= 0 && idx + num < p->sc->nmats)
        {
            idx += num;
            p->selected_mesh->mat = p->sc->mats[idx];

            char s[20] = { 0 };
            sprintf(s, "%d", idx);
            bscale_update_tex(p->toolbar->obj_mat_scale, s, p->rend, p->font);
        }
    }
}

void decrease_mat_idx(struct Prog *p)
{
    modify_mat_idx(p, -1);
}

void increase_mat_idx(struct Prog *p)
{
    modify_mat_idx(p, 1);
}

void update_edit_mat_info(struct Toolbar *t)
{
    struct Slider **arr = t->mat_props;
    struct Material *m = t->p->sc->mats[t->mat_i];

    arr[0]->value = m->col.x;
    arr[1]->value = m->col.y;
    arr[2]->value = m->col.z;

    arr[3]->value = m->ref_diffuse;
    arr[4]->value = m->ref_specular;
    arr[5]->value = m->ref_mirror;

    arr[6]->value = m->specular_exp;

    for (int i = 0; i < 7; ++i)
        slider_redo_tex(arr[i], t->p->rend, t->p->font);
}

void modify_edit_mat_idx(struct Prog *p, int num)
{
    int idx = p->toolbar->mat_i;

    if (idx + num >= 0 && idx + num < p->sc->nmats)
    {
        idx += num;
        p->toolbar->mat_i = idx;

        char s[20] = { 0 };
        sprintf(s, "%d", idx);
        bscale_update_tex(p->toolbar->mat_scale, s, p->rend, p->font);

        update_edit_mat_info(p->toolbar);

        p->toolbar->mat_preview->spheres[0]->mat = p->sc->mats[p->toolbar->mat_i];
    }
}

void decrease_edit_mat_idx(struct Prog *p)
{
    modify_edit_mat_idx(p, -1);
}

void increase_edit_mat_idx(struct Prog *p)
{
    modify_edit_mat_idx(p, 1);
}

void add_mat(struct Prog *p)
{
    p->sc->mats = realloc(p->sc->mats, sizeof(struct Material*) * ++p->sc->nmats);
    p->sc->mats[p->sc->nmats - 1] = mat_alloc((Vec3f){ 0.f, 0.f, 0.f }, 0.f, 0.f, 0.f, 1.f, 0);
}

void add_light(struct Prog *p)
{
    p->sc->lights = realloc(p->sc->lights, sizeof(struct Light*) * ++p->sc->nlights);
    p->sc->lights[p->sc->nlights - 1] = light_alloc(vec_addv(p->sc->cam->pos, rasterize_rotate_cc((Vec3f){ 0.f, 0.f, 1.f }, p->sc->cam->angle)), 0.f);

    p->lights = realloc(p->lights, sizeof(struct Mesh*) * ++p->nlights);
    p->lights[p->nlights - 1] = mesh_alloc(p->sc->lights[p->sc->nlights - 1]->pos, (Vec3f){ 0.f, 0.f, 0.f }, "res/light.obj", 0);
    p->lights[p->nlights - 1]->col = (SDL_Color){ 239, 255, 166 };
}

struct Toolbar *toolbar_alloc(struct Prog *p)
{
    struct Toolbar *t = malloc(sizeof(struct Toolbar));
    t->p = p;

    strcpy(t->obj, "None");
    t->obj_tex = util_render_text(p->rend, p->font, "None", (SDL_Color){ 255, 255, 255 });

    SDL_Point ssize = util_ssize(t->p->window);
    t->nbuttons = 2;
    t->buttons = malloc(sizeof(struct Button*) * t->nbuttons);

    t->obj_y = 50;
    t->mat_y = 210;

    t->selected_slider = 0;
    t->pressed_button = 0;

    // OBJECT
    char *labels[6] = { "x: ", "y: ", "z: ", "yaw: ", "pitch: ", "roll: " };
    for (int i = 0; i < 3; ++i)
        t->obj_props[i] = slider_alloc((SDL_Point){ ssize.x + 10, t->obj_y + i * 30 }, .01f, 0.f, labels[i], t->p->rend, t->p->font);

    for (int i = 3; i < 6; ++i)
        t->obj_props[i] = slider_alloc((SDL_Point){ ssize.x + 10 + 100 + 20, t->obj_y + (i - 3) * 30 }, .01f, 0.f, labels[i], t->p->rend, t->p->font);

    t->obj_mat_scale = bscale_alloc((SDL_Point){ ssize.x + 10, t->obj_y + 100 }, "Object material", "0",
        decrease_mat_idx, increase_mat_idx, p->rend, p->font);

    // LIGHT
    t->light_in = slider_alloc((SDL_Point){ t->obj_props[0]->rect.x + 100, t->obj_props[0]->rect.y }, .01f, 0.f, "in: ", p->rend, p->font);
    t->buttons[0] = button_alloc((SDL_Rect){ 10, 400, 90, 20 }, add_light, "New light", t->p->rend, t->p->font);

    // MATERIAL
    t->mat_i = 0;
    t->mat_scale = bscale_alloc((SDL_Point){ ssize.x + 10, t->mat_y }, "Material editor", "0",
        decrease_edit_mat_idx, increase_edit_mat_idx, p->rend, p->font);

    char *mlabels[7] = { "r: ", "g: ", "b: ", "diff: ", "spec: ", "mirr: ", "exp: " };
    for (int i = 0; i < 3; ++i)
        t->mat_props[i] = slider_alloc((SDL_Point){ ssize.x + 10, 60 + t->mat_y + i * 30 }, .01f, 0.f, mlabels[i], t->p->rend, t->p->font);

    for (int i = 3; i < 6; ++i)
        t->mat_props[i] = slider_alloc((SDL_Point){ ssize.x + 10 + 120, 60 + t->mat_y + (i - 3) * 30 }, .01f, 0.f, mlabels[i], t->p->rend, t->p->font);

    t->mat_props[6] = slider_alloc((SDL_Point){ ssize.x + 10 + 60, 60 + t->mat_y + 90 }, .4f, 0.f, mlabels[6], t->p->rend, t->p->font);

    update_edit_mat_info(t);

    t->mat_preview = scene_alloc("res/mat_preview");
    t->mat_preview->spheres[0]->mat = t->p->sc->mats[0];
    
    t->buttons[1] = button_alloc((SDL_Rect){ t->mat_props[3]->rect.x - ssize.x + 20, t->mat_props[3]->rect.y - 115, 60, 20 }, add_mat, "New mat", p->rend, p->font);

    // SCALES
    t->nscales = 2;
    t->scales = malloc(sizeof(struct Bscale*) * t->nscales);

    t->scales[0] = t->obj_mat_scale;
    t->scales[1] = t->mat_scale;

    return t;
}


void toolbar_free(struct Toolbar *t)
{
    for (int i = 0; i < 6; ++i)
        slider_free(t->obj_props[i]);

    for (int i = 0; i < 7; ++i)
        slider_free(t->mat_props[i]);

    slider_free(t->light_in);

    if (t->obj_tex)
        SDL_DestroyTexture(t->obj_tex);

    for (size_t i = 0; i < t->nbuttons; ++i)
        button_free(t->buttons[i]);

    free(t->buttons);

    for (size_t i = 0; i < t->nscales; ++i)
        bscale_free(t->scales[i]);

    free(t->scales);

    scene_free(t->mat_preview);

    free(t);
}


void toolbar_render(struct Toolbar *t)
{
    SDL_Point ssize = util_ssize(t->p->window);
    SDL_SetRenderDrawColor(t->p->rend, 40, 40, 40, 255);
    SDL_Rect r = { ssize.x, 0, EDITOR_TOOLBAR_WIDTH, ssize.y };
    SDL_RenderFillRect(t->p->rend, &r);

    // OBJECT
    SDL_Rect obj_r = { ssize.x + 10, 10 };
    SDL_QueryTexture(t->obj_tex, 0, 0, &obj_r.w, &obj_r.h);
    SDL_RenderCopy(t->p->rend, t->obj_tex, 0, &obj_r);

    for (int i = 0; i < 6; ++i)
        slider_render(t->obj_props[i], t->p->rend);

    if (t->p->selected_type == OBJ_LIGHT)
        bscale_update_tex(t->obj_mat_scale, "NaN", t->p->rend, t->p->font);

    bscale_render(t->obj_mat_scale, t->p->rend);

    // LIGHT
    slider_render(t->light_in, t->p->rend);

    // MATERIAL
    bscale_render(t->mat_scale, t->p->rend);

    for (int i = 0; i < 7; ++i)
        slider_render(t->mat_props[i], t->p->rend);

    t->mat_preview->sleep_time = 0;
    util_set_loglevel(LOG_SILENT);
    Vec3f *frame = render_rend(t->mat_preview);
    t->mat_preview->sleep_time = 1;

    for (int y = 0; y < 72; ++y)
    {
        for (int x = 0; x < 72; ++x)
        {
            Vec3f pix = frame[y * 72 + x];
            pix.x = CLAMP(pix.x, 0.f, 1.f);
            pix.y = CLAMP(pix.y, 0.f, 1.f);
            pix.z = CLAMP(pix.z, 0.f, 1.f);

            SDL_SetRenderDrawColor(t->p->rend, pix.x * 255.f, pix.y * 255.f, pix.z * 255.f, 255);
            SDL_RenderDrawPoint(t->p->rend, t->mat_props[3]->rect.x + 20 + x, t->mat_props[3]->rect.y - 80 + y);
        }
    }

    free(frame);

    // BUTTONS
    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);

    for (size_t i = 0; i < t->nbuttons; ++i)
    {
        t->buttons[i]->rect.x += ssize.x;
        button_render(t->buttons[i], t->p->rend, mouse);
        t->buttons[i]->rect.x -= ssize.x;
    }
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

            if (t->p->selected_type == OBJ_MESH)
            {
                arr[3]->value = m->rot.x;
                arr[4]->value = m->rot.y;
                arr[5]->value = m->rot.z;
            }
            else if (t->p->selected_type == OBJ_LIGHT)
            {
                t->light_in->value = t->p->selected_light->in;
            }

            char s[20] = { 0 };
            sprintf(s, "%zu", scene_mat_idx(t->p->sc, t->p->selected_mesh->mat));
            bscale_update_tex(t->obj_mat_scale, s, t->p->rend, t->p->font);
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

            if (t->p->selected_type == OBJ_MESH)
            {
                m->rot = (Vec3f){
                    arr[3]->value,
                    arr[4]->value,
                    arr[5]->value
                };
            }
            else if (t->p->selected_type == OBJ_LIGHT)
            {
                t->p->selected_light->pos = m->pos;
                t->p->selected_light->in = t->light_in->value;
            }
        }
    }

    struct Slider **ms = t->mat_props;
    struct Material *mt = t->p->sc->mats[t->mat_i];

    mt->col = (Vec3f){
        ms[0]->value, ms[1]->value, ms[2]->value
    };

    mt->ref_diffuse = ms[3]->value;
    mt->ref_specular = ms[4]->value;
    mt->ref_mirror = ms[5]->value;
    mt->specular_exp = ms[6]->value;

    for (int i = 0; i < 6; ++i)
        slider_redo_tex(t->obj_props[i], t->p->rend, t->p->font);

    slider_redo_tex(t->light_in, t->p->rend, t->p->font);
}


void toolbar_update_positions(struct Toolbar *t)
{
    SDL_Point ssize = util_ssize(t->p->window);

    t->light_in->rect.x = ssize.x + 1000;

    for (int i = 0; i < 3; ++i)
        t->obj_props[i]->rect.x = ssize.x + 10;

    if (t->p->selected_type == OBJ_MESH)
    {
        for (int i = 3; i < 6; ++i)
            t->obj_props[i]->rect.x = ssize.x + 10 + 120;
    }
    else if (t->p->selected_type == OBJ_LIGHT)
    {
        for (int i = 3; i < 6; ++i)
            t->obj_props[i]->rect.x = ssize.x + 1000;

        t->light_in->rect.x = ssize.x + 10 + 120;
    }

    for (int i = 0; i < 3; ++i)
        t->mat_props[i]->rect.x = ssize.x + 10;

    for (int i = 3; i < 6; ++i)
        t->mat_props[i]->rect.x = ssize.x + 10 + 120;

    t->mat_props[6]->rect.x = ssize.x + 10 + 60;

    for (size_t i = 0; i < t->nscales; ++i)
        bscale_update_pos(t->scales[i], ssize.x + 10);
}


bool toolbar_slide_sliders(struct Toolbar *t, int pixels)
{
    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);

    if (t->selected_slider)
    {
        slider_slide(t->selected_slider, pixels, t->p->rend, t->p->font);
    }
    else
    {
        if (t->p->selected_mesh)
        {
            for (int i = 0; i < 6; ++i)
            {
                if (t->selected_slider == t->obj_props[i])
                    if (i >= 3)
                        t->selected_slider->value = util_restrict_angle(t->selected_slider->value);

                if (util_point_in_rect(mouse, t->obj_props[i]->rect))
                    t->selected_slider = t->obj_props[i];
            }
        }

        for (int i = 0; i < 7; ++i)
        {
            if (util_point_in_rect(mouse, t->mat_props[i]->rect))
            {
                slider_slide(t->mat_props[i], pixels, t->p->rend, t->p->font);
                t->selected_slider = t->mat_props[i];
            }
        }

        if (util_point_in_rect(mouse, t->light_in->rect))
        {
            t->selected_slider = t->light_in;
        }
    }

    for (int i = 0; i < 7; ++i)
    {
        float prev = t->mat_props[i]->value;

        if (i < 3 || i == 5)
            t->mat_props[i]->value = fmin(t->mat_props[i]->value, 1.f);

        t->mat_props[i]->value = fmax(0.f, t->mat_props[i]->value);

        if (prev != t->mat_props[i]->value)
        {
            slider_redo_tex(t->mat_props[i], t->p->rend, t->p->font);
        }
    }

    return t->selected_slider != 0;
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

    for (size_t i = 0; i < t->nscales; ++i)
    {
        struct Button *b = bscale_click(t->scales[i], mouse, t->p);

        if (b)
        {
            t->pressed_button = b;
            break;
        }
    }
}


void toolbar_set_edited_mat(struct Prog *p, int idx)
{
    modify_edit_mat_idx(p, idx - p->toolbar->mat_i);
}

