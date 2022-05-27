#ifndef EDITOR_TOOLBAR_H
#define EDITOR_TOOLBAR_H

#include "button.h"
#include "slider.h"

struct Prog;

struct Toolbar
{
    SDL_Window *win;

    struct Slider *obj_pos[3];
};

struct Toolbar *toolbar_alloc(SDL_Window *w, SDL_Renderer *rend, TTF_Font *font);
void toolbar_free(struct Toolbar *t);

void toolbar_render(struct Toolbar *t, SDL_Renderer *rend);

void toolbar_main(struct Toolbar *t, struct Prog *p);
void toolbar_update_positions(struct Toolbar *t);

void toolbar_slide_sliders(struct Toolbar *t, struct Prog *p, int pixels);

#endif

