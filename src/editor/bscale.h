#ifndef EDITOR_BSCALE_H
#define EDITOR_BSCALE_H

#include "button.h"

struct Prog;

struct Bscale
{
    int y;

    SDL_Texture *label, *value;
    struct Button *buttons[2];
};

struct Bscale *bscale_alloc(SDL_Point pos, const char *label, const char *value,
    void (*min)(struct Prog *p), void (*add)(struct Prog *p), SDL_Renderer *rend, TTF_Font *font);
void bscale_free(struct Bscale *b);

void bscale_render(struct Bscale *b, SDL_Renderer *rend);
struct Button *bscale_click(struct Bscale *b, SDL_Point mouse, struct Prog *p);

void bscale_update_tex(struct Bscale *b, const char *value, SDL_Renderer *rend, TTF_Font *font);
void bscale_update_pos(struct Bscale *b, int x);

#endif

