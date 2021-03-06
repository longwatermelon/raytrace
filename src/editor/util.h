#ifndef EDITOR_UTIL_H
#define EDITOR_UTIL_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define EDITOR_TOOLBAR_WIDTH 250

#define RM_ELEM(arr, i, len, type) { memmove(arr + i, arr + i + 1, (--len - i) * sizeof(type)); }

// get size of scene preview
SDL_Point util_ssize(SDL_Window *w);

bool util_point_in_rect(SDL_Point p, SDL_Rect r);

SDL_Texture *util_render_text(SDL_Renderer* rend, TTF_Font* font, const char* text, SDL_Color color);

void util_strcat(char **dst, char *src);

float util_timediff(struct timespec *t1, struct timespec *t2);

float util_restrict_angle(float angle);

char *util_parent(const char *file);

#endif

