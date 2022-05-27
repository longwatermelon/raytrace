#ifndef EDITOR_TOOLBAR_H
#define EDITOR_TOOLBAR_H

#include "button.h"
#include "slider.h"

struct Prog;

struct Toolbar
{
    struct Prog *p;

    struct Slider *obj_pos[3];
};

struct Toolbar *toolbar_alloc(struct Prog *p);
void toolbar_free(struct Toolbar *t);

void toolbar_render(struct Toolbar *t);

void toolbar_main(struct Toolbar *t);
void toolbar_update_positions(struct Toolbar *t);

void toolbar_slide_sliders(struct Toolbar *t, int pixels);

#endif

