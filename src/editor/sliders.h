#ifndef EDITOR_SLIDERS_H
#define EDITOR_SLIDERS_H

#include "slider.h"
#include <SDL2/SDL.h>

struct SliderWrapper
{
    struct Slider *s;
    float *value;
    int default_offset;
};

struct SliderWrapper *sw_alloc(struct Slider *s, float *value, int default_offset);
void sw_free(struct SliderWrapper *sw);

void sw_render(struct SliderWrapper *sw, SDL_Renderer *rend);
// x: ssize.x
void sw_update_pos(struct SliderWrapper *sw, int x);

void sw_slide(struct SliderWrapper *sw, int px, SDL_Renderer *rend, TTF_Font *font);

struct Sliders
{
    struct SliderWrapper **sliders;
    size_t nsliders;

    struct Slider **psliders;
    float **pvalues;
};

// All arrays passed will be freed in sliders_free()
struct Sliders *sliders_alloc(struct Slider **sliders, float **values, size_t count, int ssize_x);
void sliders_free(struct Sliders *s);

void sliders_render(struct Sliders *s, SDL_Renderer *rend);
// x: ssize.x
void sliders_update_pos(struct Sliders *s, int x);

// Find slider wrapper that mouse is currently over
struct SliderWrapper *sliders_slider(struct Sliders *s);

// Update SliderWrapper values according to Sliders::pvalues
void sliders_update_values(struct Sliders *s, SDL_Renderer *rend, TTF_Font *font);

#endif

