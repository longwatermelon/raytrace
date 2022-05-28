#ifndef EDITOR_TOOLBAR_H
#define EDITOR_TOOLBAR_H

#include "button.h"
#include "slider.h"
#include <limits.h>

struct Prog;

struct Toolbar
{
    struct Prog *p;

    char obj[PATH_MAX];
    SDL_Texture *obj_tex;

    struct Slider *selected_slider;
    struct Button *pressed_button;

    struct Button **buttons;
    size_t nbuttons;

    // Object position sliders
    struct Slider *obj_props[6];

    // Thread number customization
    SDL_Texture *threads_text, *threads_num;
};

struct Toolbar *toolbar_alloc(struct Prog *p);
void toolbar_free(struct Toolbar *t);

void toolbar_render(struct Toolbar *t);

void toolbar_main(struct Toolbar *t);
void toolbar_update_positions(struct Toolbar *t);

bool toolbar_slide_sliders(struct Toolbar *t, int pixels, bool ignore_accuracy);

void toolbar_buttons_pressed(struct Toolbar *t, SDL_Point mouse);

#endif

