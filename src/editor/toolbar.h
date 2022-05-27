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

    struct Slider *obj_pos[3];

    struct Slider *selected_slider;
};

struct Toolbar *toolbar_alloc(struct Prog *p);
void toolbar_free(struct Toolbar *t);

void toolbar_render(struct Toolbar *t);

void toolbar_main(struct Toolbar *t);
void toolbar_update_positions(struct Toolbar *t);

bool toolbar_slide_sliders(struct Toolbar *t, int pixels, bool ignore_accuracy);

#endif

