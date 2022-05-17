#include "util.h"


SDL_Point util_ssize(SDL_Window *w)
{
    SDL_Point size;
    SDL_GetWindowSize(w, &size.x, &size.y);

    size.x -= EDITOR_TOOLBAR_WIDTH;
    return size;
}

