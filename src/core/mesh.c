#include "mesh.h"
#include "util.h"
#include "rasterize.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


struct Mesh *mesh_alloc(Vec3f pos, Vec3f rot, const char *fp, struct Material *mat)
{
    struct Mesh *m = malloc(sizeof(struct Mesh));
    m->pos = pos;
    m->rot = rot;

    m->pts = 0;
    m->npts = 0;

    m->tris = 0;
    m->ntris = 0;

    m->norms = 0;
    m->nnorms = 0;

    m->mat = mat;

    if (fp)
        mesh_read(m, fp);

    strcpy(m->name, fp);
    m->bounded = false;

    m->col = (SDL_Color){ 255, 255, 255 };

    return m;
}


void mesh_free(struct Mesh *m)
{
    free(m->pts);
    free(m->tris);
    free(m->norms);
    free(m);
}


void mesh_read(struct Mesh *m, const char *fp)
{
    FILE *f = fopen(fp, "r");

    if (!f)
    {
        fprintf(stderr, "Error: couldn't open file '%s'.\n", fp);
        exit(EXIT_FAILURE);
    }

    char *line = 0;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, f)) != -1)
    {
        if (line[0] == 'v')
        {
            if (line[1] == ' ')
            {
                m->pts = realloc(m->pts, sizeof(Vec3f) * ++m->npts);
                Vec3f *p = &m->pts[m->npts - 1];

                sscanf(line, "%*s %f %f %f", &p->x, &p->y, &p->z);
            }
            else if (line[1] == 'n')
            {
                m->norms = realloc(m->norms, sizeof(Vec3f) * ++m->nnorms);
                Vec3f *n = &m->norms[m->nnorms - 1];

                sscanf(line, "%*s %f %f %f", &n->x, &n->y, &n->z);
            }
        }
        else if (line[0] == 'f')
        {
            int ws = 0;

            for (int i = 0; i < strlen(line); ++i)
            {
                if (line[i] == ' ')
                    ++ws;
            }

            if (ws != 3)
            {
                fprintf(stderr, "Error in file '%s': faces must be triangulated.\n", fp);
                exit(EXIT_FAILURE);
            }

            m->tris = realloc(m->tris, sizeof(Triangle) * ++m->ntris);

            int idx[3];
            int norm;
            sscanf(line, "%*s %d/%*d/%d %d%*s %d%*s", idx, &norm, idx + 1, idx + 2);

            m->tris[m->ntris - 1].idx[0] = idx[0] - 1;
            m->tris[m->ntris - 1].idx[1] = idx[1] - 1;
            m->tris[m->ntris - 1].idx[2] = idx[2] - 1;

            m->tris[m->ntris - 1].nidx = norm - 1;
        }
    }

    free(line);
    fclose(f);
}


bool mesh_ray_intersect(struct Mesh *m, Vec3f ro, Vec3f rdir, Uint32 opt, float *t, Triangle *tri)
{
    float nearest = INFINITY;
    *t = INFINITY;

    for (size_t i = 0; i < m->ntris; ++i)
    {
        if (opt & OPT_BACKFACE_CULLING && vec_dot(rdir, rasterize_rotate_cc(m->norms[m->tris[i].nidx], m->rot)) > 0.f)
            continue;

        if (mesh_ray_tri_intersect(m, m->tris[i], ro, rdir, &nearest))
        {
            if (nearest < *t)
            {
                *t = nearest;
                *tri = m->tris[i];
            }
        }
    }

    return *t != INFINITY;
}


bool mesh_ray_tri_intersect(struct Mesh *m, Triangle tri, Vec3f ro, Vec3f rdir, float *t)
{
    // find intersection point
    Vec3f a = rasterize_rotate_cc(m->pts[tri.idx[0]], m->rot);
    a = vec_addv(a, m->pos);

    Vec3f b = rasterize_rotate_cc(m->pts[tri.idx[1]], m->rot);
    b = vec_addv(b, m->pos);

    Vec3f c = rasterize_rotate_cc(m->pts[tri.idx[2]], m->rot);
    c = vec_addv(c, m->pos);

    Vec3f norm = rasterize_rotate_cc(m->norms[tri.nidx], m->rot);
    *t = (vec_dot(a, norm) - vec_dot(ro, norm)) / vec_dot(rdir, norm);

    // check if inside triangle
    Vec3f p = vec_addv(ro, vec_mulf(rdir, *t));

    Vec3f ca = vec_sub(c, a);
    Vec3f ba = vec_sub(b, a);

    float a1 = vec_dot(ba, ba);
    float b1 = vec_dot(ca, ba);
    float b2 = vec_dot(ca, ca);

    float c1 = vec_dot(ba, vec_sub(p, a));
    float c2 = vec_dot(ca, vec_sub(p, a));

    float y = ((c1 * b1) - (c2 * a1)) / ((b1 * b1) - (a1 * b2));
    float x = (c1 - (b1 * y)) / a1;

    return (x >= 0.f && x <= 1.f && y >= 0.f && y <= 1.f && x + y >= 0.f && x + y <= 1.f && *t >= 0.f);
}


void mesh_find_bounds(struct Mesh *m, struct Camera *cam)
{
    m->bounded = true;

    m->min = (Point){ INT_MIN, INT_MIN };
    m->max = (Point){ INT_MAX, INT_MAX };

    for (size_t i = 0; i < m->npts; ++i)
    {
        Vec3f adjusted = vec_sub(vec_addv(rasterize_rotate_cc(m->pts[i], m->rot), m->pos), cam->pos);
        adjusted = rasterize_rotate_ccw(adjusted, cam->angle);

        SDL_Point p = rasterize_project_point(adjusted, 1000, 1000);

        if (p.x < m->min.x)
            m->min.x = p.x;

        if (p.x > m->max.x)
            m->max.x = p.x;

        if (p.y < m->min.y)
            m->min.y = p.y;

        if (p.y > m->max.y)
            m->max.y = p.y;
    }

#if 0
    m->top_ry = rasterize_rotate_cc(vec_normalize(t), cam->angle).y;
    m->bot_ry = rasterize_rotate_cc(vec_normalize(b), cam->angle).y;
    m->left_rx = rasterize_rotate_cc(vec_normalize(l), cam->angle).x;
    m->right_rx = rasterize_rotate_cc(vec_normalize(r), cam->angle).x;
#endif
}
