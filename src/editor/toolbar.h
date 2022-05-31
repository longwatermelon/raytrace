#ifndef EDITOR_TOOLBAR_H
#define EDITOR_TOOLBAR_H

#include "button.h"
#include "bscale.h"
#include "slider.h"
#include "sliders.h"
#include <limits.h>

struct Prog;

struct Toolbar
{
    struct Prog *p;

    char obj[PATH_MAX];
    SDL_Texture *obj_tex;

    struct SliderWrapper *selected_slider;
    struct Button *pressed_button;

    struct Button **buttons;
    size_t nbuttons;

    // Object modifiers
    int obj_y;
    struct Sliders *obj_sliders;
    struct Bscale *obj_mat_scale;

    // Light modifiers
    struct Sliders *light_sliders;

    // Material modifiers
    int mat_y;
    int mat_i;
    struct Bscale *mat_scale;
    struct Sliders *mat_sliders;

    struct Scene *mat_preview;

    // Scales
    struct Bscale **scales;
    size_t nscales;

    struct Sliders **sliders;
    size_t nsliders;
};

struct Toolbar *toolbar_alloc(struct Prog *p);
void toolbar_free(struct Toolbar *t);

void toolbar_render(struct Toolbar *t);

void toolbar_main(struct Toolbar *t);
void toolbar_update_positions(struct Toolbar *t);

bool toolbar_slide_sliders(struct Toolbar *t, int pixels);

void toolbar_buttons_pressed(struct Toolbar *t, SDL_Point mouse);

void toolbar_set_edited_mat(struct Prog *p, int idx);

#endif

