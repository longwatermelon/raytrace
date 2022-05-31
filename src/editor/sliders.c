#include "sliders.h"
#include "util.h"


struct SliderWrapper *sw_alloc(struct Slider *s, float *value, int default_offset)
{
    struct SliderWrapper *sw = malloc(sizeof(struct SliderWrapper));
    sw->s = s;
    sw->value = value;
    sw->default_offset = default_offset;

    return sw;
}


void sw_free(struct SliderWrapper *sw)
{
    slider_free(sw->s);
    free(sw);
}


void sw_render(struct SliderWrapper *sw, SDL_Renderer *rend)
{
    slider_render(sw->s, rend);
}


void sw_update_pos(struct SliderWrapper *sw, int x)
{
    sw->s->rect.x = x + sw->default_offset;
}


void sw_slide(struct SliderWrapper *sw, int px, SDL_Renderer *rend, TTF_Font *font)
{
    slider_slide(sw->s, px, rend, font);
    *sw->value = sw->s->value;
}


struct Sliders *sliders_alloc(struct Slider **sliders, float **values, size_t count, int ssize_x)
{
    struct Sliders *s = malloc(sizeof(struct Sliders));

    s->nsliders = count;
    s->sliders = malloc(sizeof(struct SliderWrapper*) * count);

    for (size_t i = 0; i < count; ++i)
        s->sliders[i] = sw_alloc(sliders[i], values[i], sliders[i]->rect.x - ssize_x);

    s->psliders = sliders;
    s->pvalues = values;

    return s;
}


void sliders_free(struct Sliders *s)
{
    for (size_t i = 0; i < s->nsliders; ++i)
        sw_free(s->sliders[i]);

    free(s->sliders);
    free(s->psliders);
    free(s->pvalues);

    free(s);
}


void sliders_render(struct Sliders *s, SDL_Renderer *rend)
{
    for (size_t i = 0; i < s->nsliders; ++i)
        sw_render(s->sliders[i], rend);
}


void sliders_update_pos(struct Sliders *s, int x)
{
    for (size_t i = 0; i < s->nsliders; ++i)
        sw_update_pos(s->sliders[i], x);
}


struct SliderWrapper *sliders_slider(struct Sliders *s)
{
    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);

    for (size_t i = 0; i < s->nsliders; ++i)
    {
        if (util_point_in_rect(mouse, s->sliders[i]->s->rect))
        {
            return s->sliders[i];
        }
    }

    return 0;
}


void sliders_update_values(struct Sliders *s, SDL_Renderer *rend, TTF_Font *font)
{
    for (size_t i = 0; i < s->nsliders; ++i)
    {
        s->sliders[i]->value = s->pvalues[i];
        s->sliders[i]->s->value = *s->pvalues[i];
        slider_redo_tex(s->sliders[i]->s, rend, font);
    }
}

