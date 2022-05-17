#include "button.h"
#include "util.h"
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


void button_render(struct Button *b, SDL_Renderer *rend, SDL_Point mouse)
{
    if (util_point_in_rect(mouse, b->rect))
        SDL_SetRenderDrawColor(rend, 80, 80, 80, 255);
    else
        SDL_SetRenderDrawColor(rend, 50, 50, 50, 255);

    SDL_RenderFillRect(rend, &b->rect);
}

