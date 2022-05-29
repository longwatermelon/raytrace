#include "explorer.h"
#include "button.h"
#include "util.h"
#include "prog.h"
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <SDL2/SDL_image.h>

void close_explorer(struct Prog *p)
{
    p->explorer->running = false;
}

void select_node(struct Prog *p)
{
    if (p->explorer->selected)
    {
        char *s = calloc(sizeof(char), strlen(p->explorer->dir) + strlen(p->explorer->selected->path) + 2);
        sprintf(s, "%s/%s", p->explorer->dir, p->explorer->selected->path);
        p->explorer->selected_path = s;
        p->explorer->running = false;
    }
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

    e->selected = 0;

    char buf[PATH_MAX];
    char *full = realpath(dir, buf);
    strcpy(e->dir, full);

    e->folder_tex = IMG_LoadTexture(e->rend, "res/folder.png");
    e->file_tex = IMG_LoadTexture(e->rend, "res/file.png");

    e->nodes = explorer_read_dir(e, dir, &e->nodes_num);
    explorer_sort(e);

    clock_gettime(CLOCK_MONOTONIC, &e->last_click);

    e->top_y = 0;

    return e;
}


void explorer_free(struct Explorer *e)
{
    SDL_DestroyRenderer(e->rend);
    SDL_DestroyWindow(e->win);

    for (size_t i = 0; i < e->nodes_num; ++i)
        enode_free(e->nodes[i]);

    free(e->nodes);

    if (e->selected_path)
        free(e->selected_path);

    SDL_DestroyTexture(e->file_tex);
    SDL_DestroyTexture(e->folder_tex);

    free(e);
}


char *explorer_find(struct Explorer *e)
{
    e->selected_path = 0;
    SDL_Event evt;

    struct Button *buttons[] = {
        button_alloc((SDL_Rect){ 450, 550, 150, 20 }, close_explorer, "Close", e->rend, e->font),
        button_alloc((SDL_Rect){ 620, 550, 150, 20 }, select_node, "Select file", e->rend, e->font)
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

                if (!pressed)
                {
                    if (mouse.y < 450 && mouse.x < 500)
                    {
                        e->selected = explorer_find_node(e, mouse);

                        struct timespec now;
                        clock_gettime(CLOCK_MONOTONIC, &now);

                        if (util_timediff(&e->last_click, &now) < .5f && e->selected)
                        {
                            if (e->selected->type == DT_DIR)
                            {
                                if (strcmp(e->selected->path, "..") == 0)
                                {
                                    char *parent = util_parent(e->dir);
                                    strcpy(e->dir, parent);
                                    free(parent);
                                }
                                else
                                {
                                    char *s = calloc(sizeof(char), strlen(e->dir) + strlen(e->selected->path) + 2);
                                    sprintf(s, "%s/%s", e->dir, e->selected->path);
                                    strcpy(e->dir, s);
                                    free(s);
                                }

                                for (size_t i = 0; i < e->nodes_num; ++i)
                                    enode_free(e->nodes[i]);

                                free(e->nodes);
                                e->nodes = explorer_read_dir(e, e->dir, &e->nodes_num);
                                explorer_sort(e);
                                e->top_y = 0;
                            }
                        }
                        else
                        {
                            clock_gettime(CLOCK_MONOTONIC, &e->last_click);
                        }
                    }
                    else
                    {
                        e->selected = 0;
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
        
            case SDL_MOUSEWHEEL:
            {
                if ((evt.wheel.y > 0 && e->top_y + evt.wheel.y * 20 <= 0) ||
                    (evt.wheel.y < 0 && e->nodes_num * 20 + e->top_y >= 450))
                {
                    e->top_y += evt.wheel.y * 20;
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

    return e->selected_path;
}


void explorer_render_nodes(struct Explorer *e)
{
    for (size_t i = 0; i < e->nodes_num; ++i)
    {
        SDL_Rect rtex = { 20, e->top_y + i * 20 };
        TTF_SizeText(e->font, " ", &rtex.w, &rtex.h);
        rtex.w *= 2;

        SDL_Texture *tex = e->nodes[i]->type == DT_REG ? e->file_tex : e->folder_tex;
        SDL_RenderCopy(e->rend, tex, 0, &rtex);

        SDL_Rect r = { 50, e->top_y + i * 20 };
        SDL_QueryTexture(e->nodes[i]->tex, 0, 0, &r.w, &r.h);
        SDL_RenderCopy(e->rend, e->nodes[i]->tex, 0, &r);

        if (e->selected == e->nodes[i])
        {
            r.x = 20;
            r.w = 500;
            SDL_SetRenderDrawBlendMode(e->rend, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(e->rend, 255, 255, 255, 100);
            SDL_RenderFillRect(e->rend, &r);
            SDL_SetRenderDrawBlendMode(e->rend, SDL_BLENDMODE_NONE);
        }
    }
}


struct ENode *explorer_find_node(struct Explorer *e, SDL_Point mouse)
{
    for (size_t i = 0; i < e->nodes_num; ++i)
    {
        int y = e->top_y + i * 20;
        
        if (mouse.y > y && mouse.y < y + 20)
        {
            return e->nodes[i];
        }
    }

    return 0;
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
        if (strcmp(de->d_name, ".") == 0)
            continue;

        if (de->d_type == DT_REG || de->d_type == DT_DIR)
        {
            nodes = realloc(nodes, sizeof(struct ENode*) * ++*num);

            nodes[*num - 1] = enode_alloc(de->d_name, de->d_type);
            nodes[*num - 1]->tex = util_render_text(e->rend, e->font, de->d_name, (SDL_Color){ 255, 255, 255 });
        }
    }

    closedir(d);
    return nodes;
}


void explorer_sort(struct Explorer *e)
{
    struct ENode **dirs = 0;
    size_t ndirs = 0;

    struct ENode **files = 0;
    size_t nfiles = 0;

    for (size_t i = 0; i < e->nodes_num; ++i)
    {
        if (e->nodes[i]->type == DT_DIR)
        {
            dirs = realloc(dirs, sizeof(struct ENode*) * ++ndirs);
            dirs[ndirs - 1] = e->nodes[i];
        }
        else if (e->nodes[i]->type == DT_REG)
        {
            files = realloc(files, sizeof(struct ENode*) * ++nfiles);
            files[nfiles - 1] = e->nodes[i];
        }
    }

    explorer_sort_alpha(dirs, ndirs);
    explorer_sort_alpha(files, nfiles);

    struct ENode **nodes = malloc(sizeof(struct ENode*) * (ndirs + nfiles));

    memcpy(nodes, dirs, ndirs * sizeof(struct ENode*));
    memcpy(&nodes[ndirs], files, nfiles * sizeof(struct ENode*));

    free(e->nodes);
    e->nodes = nodes;

    free(dirs);
    free(files);
}


void explorer_sort_alpha(struct ENode **nodes, size_t len)
{
    for (size_t i = 0; i < len; ++i)
    {
        for (int j = i + 1; j < len; ++j)
        {
            if (strcmp(nodes[i]->path, nodes[j]->path) > 0)
            {
                struct ENode *tmp = nodes[i];
                nodes[i] = nodes[j];
                nodes[j] = tmp;
            }
        }
    }
}

