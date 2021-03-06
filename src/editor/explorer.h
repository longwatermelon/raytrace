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

    struct ENode *selected;
    char *selected_path;

    SDL_Texture *folder_tex, *file_tex;

    struct timespec last_click;

    int top_y;
};

struct Explorer *explorer_alloc(const char *dir, struct Prog *p);
void explorer_free(struct Explorer *e);

char *explorer_find(struct Explorer *e);
void explorer_render_nodes(struct Explorer *e);
struct ENode *explorer_find_node(struct Explorer *e, SDL_Point mouse);

struct ENode **explorer_read_dir(struct Explorer *e, const char *dir, size_t *num);

void explorer_sort(struct Explorer *e);
void explorer_sort_alpha(struct ENode **nodes, size_t len);

#endif

