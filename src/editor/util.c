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


SDL_Texture *util_render_text(SDL_Renderer* rend, TTF_Font* font, const char* text, SDL_Color color)
{
    if (!text[0])
        return 0;

    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text, color);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surf);

    SDL_FreeSurface(surf);

    return tex;
}


void util_strcat(char **dst, char *src)
{
    *dst = realloc(*dst, sizeof(char) * (strlen(*dst) + strlen(src) + 1));
    strcat(*dst, src);
}


float util_timediff(struct timespec *t1, struct timespec *t2)
{
    return (t2->tv_sec - t1->tv_sec) + (t2->tv_nsec - t1->tv_nsec) / 1e9;
}


float util_restrict_angle(float angle)
{
    if (angle > 2.f * M_PI)
        angle -= 2.f * M_PI;

    if (angle < 0.f)
        angle += 2.f * M_PI;

    return angle;
}


char *util_parent(const char *file)
{
    for (int i = strlen(file) - 1; i >= 0; --i)
    {
        if (file[i] == '/')
        {
            char *parent = malloc(sizeof(char) * (i + 1));
            snprintf(parent, i + 1, "%s", file);
            return parent;
        }
    }

    return 0;
}

