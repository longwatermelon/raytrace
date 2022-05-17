#ifndef EDITOR_UTIL_H
#define EDITOR_UTIL_H

#include <stdbool.h>
#include <SDL2/SDL.h>

#define EDITOR_TOOLBAR_WIDTH 250

// get size of scene preview
SDL_Point util_ssize(SDL_Window *w);

bool util_point_in_rect(SDL_Point p, SDL_Rect r);

#endif

