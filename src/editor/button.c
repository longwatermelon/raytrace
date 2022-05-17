#include "button.h"
#include <stdlib.h>


struct Button *button_alloc(SDL_Rect r, void (*callback)(struct Prog*))
{
    struct Button *b = malloc(sizeof(struct Button));
    b->callback = callback;

    return b;
}


void button_free(struct Button *b)
{
    free(b);
}


void button_render(struct Button *b, SDL_Renderer *rend)
{
    SDL_SetRenderDrawColor(rend, 50, 50, 50, 255);
    SDL_RenderFillRect(rend, &b->rect);
}


void button_check_click(struct Button *b, struct Prog *p, int mx, int my)
{
    if (mx > b->rect.x && mx < b->rect.x + b->rect.w &&
        my > b->rect.y && my < b->rect.y + b->rect.h)
    {
        b->callback(p);
    }
}

