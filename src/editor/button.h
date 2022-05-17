#ifndef EDITOR_BUTTON_H
#define EDITOR_BUTTON_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

struct Prog;

struct Button
{
    SDL_Rect rect;
    void (*callback)(struct Prog*);

    char *text;
    SDL_Texture *tex;
    SDL_Rect tex_rect;

    bool pushed;

    TTF_Font *font;
};

struct Button *button_alloc(SDL_Rect r, void (*callback)(struct Prog*), const char *text, SDL_Renderer *rend, TTF_Font *font);
void button_free(struct Button *b);

void button_render(struct Button *b, SDL_Renderer *rend, SDL_Point mouse);

void button_update_tex_rect(struct Button *b);

#endif

