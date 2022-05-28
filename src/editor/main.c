#include "prog.h"
#include <SDL2/SDL_image.h>


int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);

    SDL_Window *w = SDL_CreateWindow("Editor",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    SDL_Renderer *r = SDL_CreateRenderer(w, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    struct Prog *p = prog_alloc(w, r);
    prog_mainloop(p);
    prog_free(p);

    SDL_DestroyRenderer(r);
    SDL_DestroyWindow(w);

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}

