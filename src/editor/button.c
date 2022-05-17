#include "button.h"
#include "util.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>


struct Button *button_alloc(SDL_Rect r, void (*callback)(struct Prog*), const char *text, SDL_Renderer *rend, TTF_Font *font)
{
    struct Button *b = malloc(sizeof(struct Button));
    b->rect = r;
    b->callback = callback;

    b->text = strdup(text);
    b->tex = util_render_text(rend, font, text, (SDL_Color){ 255, 255, 255 });

    b->pushed = false;
    b->font = font;

    button_update_tex_rect(b);

    return b;
}


void button_free(struct Button *b)
{
    free(b->text);
    SDL_DestroyTexture(b->tex);
    free(b);
}


void button_render(struct Button *b, SDL_Renderer *rend, SDL_Point mouse)
{
    if (b->pushed)
        SDL_SetRenderDrawColor(rend, 30, 30, 30, 255);
    else if (util_point_in_rect(mouse, b->rect))
        SDL_SetRenderDrawColor(rend, 80, 80, 80, 255);
    else
        SDL_SetRenderDrawColor(rend, 50, 50, 50, 255);

    SDL_RenderFillRect(rend, &b->rect);

    button_update_tex_rect(b);

    SDL_RenderCopy(rend, b->tex, 0, &b->tex_rect);
}


void button_update_tex_rect(struct Button *b)
{
    TTF_SizeText(b->font, b->text, &b->tex_rect.w, &b->tex_rect.h);

    b->tex_rect.x = b->rect.x + ((b->rect.w - b->tex_rect.w) / 2);
    b->tex_rect.y = b->rect.y + ((b->rect.h - b->tex_rect.h) / 2);
}

