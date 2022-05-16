#include "render.h"
#include <core/vector.h>
#include <core/rasterize.h>

int g_size = 0;
SDL_Point g_offset = { 0, 0 };

void render_scene(struct Scene *sc, SDL_Renderer *rend)
{
    for (size_t i = 0; i < sc->nmeshes; ++i)
    {
        render_scene_mesh(sc, sc->meshes[i], rend);
    }
}


void render_scene_mesh(struct Scene *sc, struct Mesh *m, SDL_Renderer *rend)
{
    for (size_t i = 0; i < m->ntris; ++i)
    {
        SDL_Point points[3];

        for (int j = 0; j < 3; ++j)
        {
            Vec3f p = m->pts[m->tris[i].idx[j]];
            p = vec_addv(m->pos, p);
            p = vec_sub(p, sc->cam->pos);
            p = rasterize_rotate(p, vec_mulf(sc->cam->angle, -1.f));

            if (p.z < 0.f)
            {
                // TODO triangle clipping
                goto next_tri;
            }

            points[j] = rasterize_project_point(p, g_size, g_size);

            points[j].x += g_offset.x;
            points[j].y += g_offset.y;
        }

        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
        SDL_RenderDrawLine(rend, points[0].x, points[0].y, points[1].x, points[1].y);
        SDL_RenderDrawLine(rend, points[1].x, points[1].y, points[2].x, points[2].y);
        SDL_RenderDrawLine(rend, points[0].x, points[0].y, points[2].x, points[2].y);

next_tri:;
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

