#ifndef EDITOR_SLIDER_H
#define EDITOR_SLIDER_H

#include <SDL2/SDL.h>

struct Slider
{
    SDL_Rect body, slider;
    int pix_per_unit;
    int min, max, value;
};

// pos: top left corner of slider
// value: default slider value
struct Slider *slider_alloc(SDL_Point pos, int pix_per_unit, int min, int max, int value);
void slider_free(struct Slider *s);

void slider_render(struct Slider *s, SDL_Renderer *rend);

void slider_slide(struct Slider *s, int step);

#endif

