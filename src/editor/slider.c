#include "slider.h"


struct Slider *slider_alloc(SDL_Point pos, int pix_per_unit, int min, int max, int value)
{
    struct Slider *s = malloc(sizeof(struct Slider));
    s->body = (SDL_Rect){
        pos.x, pos.y,
        pix_per_unit * (max - min),
        10
    };
    s->slider = (SDL_Rect){
        pos.x + pix_per_unit * (min + value) - 2,
        pos.y - 2,
        14, 14
    };

    s->pix_per_unit = pix_per_unit;
    s->min = min;
    s->max = max;
    s->value = value;

    return s;
}


void slider_free(struct Slider *s)
{
    free(s);
}


void slider_render(struct Slider *s, SDL_Renderer *rend)
{
    SDL_SetRenderDrawColor(rend, 50, 50, 50, 255);
    SDL_RenderFillRect(rend, &s->body);

    SDL_SetRenderDrawColor(rend, 150, 150, 150, 255);
    SDL_RenderFillRect(rend, &s->slider);
}


void slider_slide(struct Slider *s, int step)
{
    if (s->value > s->min && s->value < s->max)
    {
        s->value += step;
        s->slider.x += step * s->pix_per_unit;
    }
}

