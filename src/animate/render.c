#include "render.h"
#include <core/vector.h>
#include <core/rasterize.h>

int g_size = 0;
SDL_Point g_offset = { 0, 0 };

void render_scene(struct Scene *sc, struct Mesh *selected, SDL_Renderer *rend)
{
    for (size_t i = 0; i < sc->nmeshes; ++i)
    {
        render_scene_mesh(sc, sc->meshes[i], sc->meshes[i] == selected, rend);
    }
}


void render_scene_mesh(struct Scene *sc, struct Mesh *m, bool selected, SDL_Renderer *rend)
{
    for (size_t i = 0; i < m->ntris; ++i)
    {
        SDL_Point points[3];
        bool render = true;

        for (int j = 0; j < 3; ++j)
        {
            Vec3f p = m->pts[m->tris[i].idx[j]];
            p = vec_addv(m->pos, p);
            p = vec_sub(p, sc->cam->pos);
            p = rasterize_rotate_ccw(p, sc->cam->angle);

            if (p.z <= 1.f)
            {
                render = false;
            }
            else
            {
                points[j] = rasterize_project_point(p, g_size, g_size);

                points[j].x += g_offset.x;
                points[j].y += g_offset.y;
            }
        }

        if (render)
        {
            if (selected)
                SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
            else
                SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);

            SDL_RenderDrawLine(rend, points[0].x, points[0].y, points[1].x, points[1].y);
            SDL_RenderDrawLine(rend, points[1].x, points[1].y, points[2].x, points[2].y);
            SDL_RenderDrawLine(rend, points[0].x, points[0].y, points[2].x, points[2].y);
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

