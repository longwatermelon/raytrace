#include "bscale.h"
#include "util.h"
#include <stdlib.h>

#define BTN_ITERATE for (int i = 0; i < 2; ++i)

struct Bscale *bscale_alloc(SDL_Point pos, const char *label, const char *value,
    void (*min)(struct Prog *p), void (*add)(struct Prog *p), SDL_Renderer *rend, TTF_Font *font)
{
    struct Bscale *b = malloc(sizeof(struct Bscale));
    b->y = pos.y;

    b->label = util_render_text(rend, font, label, (SDL_Color){ 255, 255, 255 });
    b->value = util_render_text(rend, font, value, (SDL_Color){ 255, 255, 255 });

    b->buttons[0] = button_alloc((SDL_Rect){ pos.x, pos.y + 30, 20, 20 }, min, "-", rend, font);
    b->buttons[1] = button_alloc((SDL_Rect){ pos.x + 90, pos.y + 30, 20, 20 }, add, "+", rend, font);

    return b;
}


void bscale_free(struct Bscale *b)
{
    SDL_DestroyTexture(b->label);
    SDL_DestroyTexture(b->value);

    BTN_ITERATE
        button_free(b->buttons[i]);

    free(b);
}


void bscale_render(struct Bscale *b, SDL_Renderer *rend)
{
    SDL_Rect r = { b->buttons[0]->rect.x, b->y };
    SDL_QueryTexture(b->label, 0, 0, &r.w, &r.h);
    SDL_RenderCopy(rend, b->label, 0, &r);

    SDL_QueryTexture(b->value, 0, 0, &r.w, &r.h);
    r.x = (b->buttons[0]->rect.x + b->buttons[1]->rect.x + b->buttons[1]->rect.w) / 2 - (r.w / 2);
    r.y = (b->buttons[0]->rect.y + b->buttons[1]->rect.y + b->buttons[1]->rect.h) / 2 - (r.h / 2);
    SDL_RenderCopy(rend, b->value, 0, &r);

    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);

    BTN_ITERATE
        button_render(b->buttons[i], rend, mouse);
}


struct Button *bscale_click(struct Bscale *b, SDL_Point mouse, struct Prog *p)
{
    for (int i = 0; i < 2; ++i)
    {
        if (util_point_in_rect(mouse, b->buttons[i]->rect))
        {
            b->buttons[i]->pushed = true;
            b->buttons[i]->callback(p);
            return b->buttons[i];
        }
    }

    return 0;
}


void bscale_update_tex(struct Bscale *b, const char *value, SDL_Renderer *rend, TTF_Font *font)
{
    if (b->value)
        SDL_DestroyTexture(b->value);

    b->value = util_render_text(rend, font, value, (SDL_Color){ 255, 255, 255 });
}


void bscale_update_pos(struct Bscale *b, int x)
{
    int diff = x - b->buttons[0]->rect.x;

    BTN_ITERATE
        b->buttons[i]->rect.x += diff;
}

