#ifndef EDITOR_UTIL_H
#define EDITOR_UTIL_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define EDITOR_TOOLBAR_WIDTH 250

// get size of scene preview
SDL_Point util_ssize(SDL_Window *w);

bool util_point_in_rect(SDL_Point p, SDL_Rect r);

SDL_Texture *util_render_text(SDL_Renderer* rend, TTF_Font* font, const char* text, SDL_Color color);

void util_strcat(char **dst, char *src);

#endif

