#ifndef EDITOR_EXPLORER_H
#define EDITOR_EXPLORER_H

#include <stdbool.h>
#include <limits.h>
#include <sys/types.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

struct Prog;

struct ENode
{
    char path[PATH_MAX];
    unsigned char type;

    SDL_Texture *tex;
};

struct ENode *enode_alloc(const char *path, unsigned char type);
void enode_free(struct ENode *en);

struct Explorer
{
    bool running;

    SDL_Window *win;
    SDL_Renderer *rend;

    TTF_Font *font;

    struct Prog *p;

    char dir[PATH_MAX];

    struct ENode **nodes;
    size_t nodes_num;
};

struct Explorer *explorer_alloc(const char *dir, struct Prog *p);
void explorer_free(struct Explorer *e);

char *explorer_find(struct Explorer *e);
void explorer_render_nodes(struct Explorer *e);

struct ENode **explorer_read_dir(struct Explorer *e, const char *dir, size_t *num);

#endif

