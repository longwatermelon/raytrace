#ifndef EDITOR_SLIDER_H
#define EDITOR_SLIDER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

struct Slider
{
    SDL_Rect rect;

    float upp; // units per pixel
    float value;

    SDL_Texture *tex;
};

struct Slider *slider_alloc(SDL_Point pos, float upp, float value, SDL_Renderer *rend, TTF_Font *font);
void slider_free(struct Slider *s);

void slider_render(struct Slider *s, SDL_Renderer *rend);

void slider_slide(struct Slider *s, int pixels, SDL_Renderer *rend, TTF_Font *font);
void slider_redo_tex(struct Slider *s, SDL_Renderer *rend, TTF_Font *font);

#endif

