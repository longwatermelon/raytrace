#ifndef EDITOR_BUTTON_H
#define EDITOR_BUTTON_H

#include <SDL2/SDL.h>

struct Prog;

struct Button
{
    SDL_Rect rect;
    void (*callback)(struct Prog*);
};

struct Button *button_alloc(SDL_Rect r, void (*callback)(struct Prog*));
void button_free(struct Button *b);

void button_render(struct Button *b, SDL_Renderer *rend, SDL_Point mouse);

#endif

