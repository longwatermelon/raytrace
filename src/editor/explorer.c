#include "explorer.h"
#include "button.h"
#include "util.h"
#include "prog.h"
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>

void close_explorer(struct Prog *p)
{
    p->explorer->running = false;
}

struct ENode *enode_alloc(const char *path, unsigned char type)
{
    struct ENode *en = malloc(sizeof(struct ENode));
    sprintf(en->path, "%s", path);
    en->type = type;

    return en;
}


void enode_free(struct ENode *en)
{
    free(en);
}


struct Explorer *explorer_alloc(const char *dir, struct Prog *p)
{
    struct Explorer *e = malloc(sizeof(struct Explorer));
    e->running = true;
    e->win = SDL_CreateWindow("Explorer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    e->rend = SDL_CreateRenderer(e->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    e->p = p;
    e->font = p->font;

    sprintf(e->dir, "%s", dir);

    e->nodes = explorer_read_dir(e, dir, &e->nodes_num);

    return e;
}


void explorer_free(struct Explorer *e)
{
    SDL_DestroyRenderer(e->rend);
    SDL_DestroyWindow(e->win);

    for (size_t i = 0; i < e->nodes_num; ++i)
        enode_free(e->nodes[i]);

    free(e->nodes);

    free(e);
}


char *explorer_find(struct Explorer *e)
{
    char *path = 0;

    SDL_Event evt;

    struct Button *buttons[] = {
        button_alloc((SDL_Rect){ 500, 550, 150, 20 }, close_explorer, "Close", e->rend, e->font)
    };
    size_t nbuttons = sizeof(buttons) / sizeof(buttons[0]);

    struct Button *pressed = 0;

    while (e->running)
    {
        SDL_Point mouse;
        SDL_GetMouseState(&mouse.x, &mouse.y);

        while (SDL_PollEvent(&evt))
        {
            switch (evt.type)
            {
            case SDL_MOUSEBUTTONDOWN:
            {
                if (!pressed)
                {
                    for (size_t i = 0; i < nbuttons; ++i)
                    {
                        if (util_point_in_rect(mouse, buttons[i]->rect))
                        {
                            pressed = buttons[i];
                            pressed->pushed = true;
                        }
                    }
                }
            } break;
            case SDL_MOUSEBUTTONUP:
            {
                if (pressed)
                {
                    pressed->callback(e->p);
                    pressed->pushed = false;
                    pressed = 0;
                }
            } break;
            }
        }

        SDL_RenderClear(e->rend);

        explorer_render_nodes(e);

        SDL_Rect bottom = { 0, 450, 800, 150 };
        SDL_SetRenderDrawColor(e->rend, 60, 60, 60, 255);
        SDL_RenderFillRect(e->rend, &bottom);

        for (size_t i = 0; i < nbuttons; ++i)
            button_render(buttons[i], e->rend, mouse);

        SDL_SetRenderDrawColor(e->rend, 30, 30, 30, 255);
        SDL_RenderPresent(e->rend);
    }

    for (size_t i = 0; i < nbuttons; ++i)
        button_free(buttons[i]);

    return path;
}


void explorer_render_nodes(struct Explorer *e)
{
    for (size_t i = 0; i < e->nodes_num; ++i)
    {
        SDL_Rect r = { 20, i * 20 };
        SDL_QueryTexture(e->nodes[i]->tex, 0, 0, &r.w, &r.h);
        SDL_RenderCopy(e->rend, e->nodes[i]->tex, 0, &r);
    }
}


struct ENode **explorer_read_dir(struct Explorer *e, const char *dir, size_t *num)
{
    struct ENode **nodes = 0;
    *num = 0;

    DIR *d = opendir(dir);

    if (!d)
    {
        fprintf(stderr, "[explorer_read_dir]: Couldn't open dir '%s'.\n", dir);
        return 0;
    }

    struct dirent *de;

    while ((de = readdir(d)) != 0)
    {
        nodes = realloc(nodes, sizeof(struct ENode*) * ++*num);

        nodes[*num - 1] = enode_alloc(de->d_name, de->d_type);
        nodes[*num - 1]->tex = util_render_text(e->rend, e->font, de->d_name, (SDL_Color){ 255, 255, 255 });
    }

    closedir(d);
    return nodes;
}

