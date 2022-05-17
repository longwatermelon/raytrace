#include "util.h"


SDL_Point util_ssize(SDL_Window *w)
{
    SDL_Point size;
    SDL_GetWindowSize(w, &size.x, &size.y);

    size.x -= EDITOR_TOOLBAR_WIDTH;
    return size;
}


bool util_point_in_rect(SDL_Point p, SDL_Rect r)
{
    return p.x > r.x && p.x < r.x + r.w &&
           p.y > r.y && p.y < r.y + r.h;
}

