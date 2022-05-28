#include "slider.h"
#include "util.h"


struct Slider *slider_alloc(SDL_Point pos, float upp, float value, const char *label, SDL_Renderer *rend, TTF_Font *font)
{
    struct Slider *s = malloc(sizeof(struct Slider));
    s->rect = (SDL_Rect){ pos.x, pos.y, 200, 20 };
    s->value = value;
    s->upp = upp;
    s->tex = 0;

    s->label = strdup(label);

    slider_redo_tex(s, rend, font);

    return s;
}


void slider_free(struct Slider *s)
{
    if (s->tex)
        SDL_DestroyTexture(s->tex);

    if (s->label)
        free(s->label);

    free(s);
}


void slider_render(struct Slider *s, SDL_Renderer *rend)
{
    SDL_SetRenderDrawColor(rend, 120, 120, 120, 255);
    SDL_RenderFillRect(rend, &s->rect);

    SDL_Point size;
    SDL_QueryTexture(s->tex, 0, 0, &size.x, &size.y);

    SDL_Rect tmp = {
        s->rect.x + (s->rect.w - size.x) / 2,
        s->rect.y + (s->rect.h - size.y) / 2,
        size.x, size.y
    };

    SDL_RenderCopy(rend, s->tex, 0, &tmp);
}


void slider_slide(struct Slider *s, int pixels, SDL_Renderer *rend, TTF_Font *font)
{
    s->value += pixels * s->upp;
    slider_redo_tex(s, rend, font);
}


void slider_redo_tex(struct Slider *s, SDL_Renderer *rend, TTF_Font *font)
{
    if (s->tex)
        SDL_DestroyTexture(s->tex);

    char tmp[100];
    sprintf(tmp, "%s%.2f", s->label, s->value);
    s->tex = util_render_text(rend, font, tmp, (SDL_Color){ 255, 255, 255 });
}

