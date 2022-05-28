#include "render.h"
#include "prog.h"
#include <core/vector.h>
#include <core/rasterize.h>

int g_size = 0;
SDL_Point g_offset = { 0, 0 };

void render_scene(struct Prog *p)
{
    for (size_t i = 0; i < p->sc->nmeshes; ++i)
    {
        render_scene_mesh(p, p->sc->meshes[i]);
    }
}


void render_scene_mesh(struct Prog *p, struct Mesh *m)
{
    for (size_t i = 0; i < m->ntris; ++i)
    {
        SDL_Point points[3];
        bool render = true;

        for (int j = 0; j < 3; ++j)
        {
            Vec3f pt = rasterize_rotate_cc(m->pts[m->tris[i].idx[j]], m->rot);
            pt = vec_addv(m->pos, pt);
            pt = vec_sub(pt, p->sc->cam->pos);
            pt = rasterize_rotate_ccw(pt, p->sc->cam->angle);

            if (pt.z <= .5f)
            {
                render = false;
            }
            else
            {
                points[j] = rasterize_project_point(pt, g_size, g_size);

                points[j].x += g_offset.x;
                points[j].y += g_offset.y;
            }
        }

        if (render)
        {
            if (m == p->selected_mesh)
                SDL_SetRenderDrawColor(p->rend, 255, 0, 0, 255);
            else if (m == p->hover_mesh)
                SDL_SetRenderDrawColor(p->rend, 0, 255, 0, 255);
            else
                SDL_SetRenderDrawColor(p->rend, 255, 255, 255, 255);

            SDL_RenderDrawLine(p->rend, points[0].x, points[0].y, points[1].x, points[1].y);
            SDL_RenderDrawLine(p->rend, points[1].x, points[1].y, points[2].x, points[2].y);
            SDL_RenderDrawLine(p->rend, points[0].x, points[0].y, points[2].x, points[2].y);
        }
    }
}


void render_set_size(int w, int h)
{
    g_size = w < h ? w : h;

    int max = w < h ? h : w;

    if (max == w)
    {
        g_offset.y = 0;
        g_offset.x = (max - h) / 2;
    }
    else
    {
        g_offset.x = 0;
        g_offset.y = (max - w) / 2;
    }
}

