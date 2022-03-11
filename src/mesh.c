#include "mesh.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


struct Mesh *mesh_alloc(Vec3f pos, const char *fp, Vec3f col)
{
    struct Mesh *m = malloc(sizeof(struct Mesh));
    m->pos = pos;

    m->pts = 0;
    m->npts = 0;

    m->tris = 0;
    m->ntris = 0;

    m->col = col;

    m->invert_normal = false;

    if (fp)
    {
        mesh_read(m, fp);
        mesh_find_bounds(m, (Vec3f){ 0.f, 0.f, 0.f });
    }

    return m;
}


void mesh_free(struct Mesh *m)
{
    free(m->pts);
    free(m->tris);
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
            m->pts = realloc(m->pts, sizeof(Vec3f) * ++m->npts);
            Vec3f *p = &m->pts[m->npts - 1];

            sscanf(line, "%*s %f %f %f", &p->x, &p->y, &p->z);
        }
        else if (line[0] == 'f')
        {
            int ws = 0;
            for (int i = 0; i < strlen(line); ++i)
            {
                if (line[i] == ' ')
                    ++ws;
            }

            if (ws == 4)
            {
                m->ntris += 2;
                m->tris = realloc(m->tris, sizeof(Triangle) * m->ntris);

                int idx[4];
                sscanf(line, "%*s %d%*s %d%*s %d%*s %d%*s", idx, idx + 1, idx + 2, idx + 3);

                m->tris[m->ntris - 2].idx[0] = idx[2] - 1;
                m->tris[m->ntris - 2].idx[1] = idx[1] - 1;
                m->tris[m->ntris - 2].idx[2] = idx[0] - 1;

                m->tris[m->ntris - 1].idx[0] = idx[2] - 1;
                m->tris[m->ntris - 1].idx[1] = idx[0] - 1;
                m->tris[m->ntris - 1].idx[2] = idx[3] - 1;
            }
            else
            {
                m->tris = realloc(m->tris, sizeof(Triangle) * ++m->ntris);

                int idx[3];
                sscanf(line, "%*s %d%*s %d%*s %d%*s", idx, idx + 1, idx + 2);

                m->tris[m->ntris - 1].idx[0] = idx[0] - 1;
                m->tris[m->ntris - 1].idx[1] = idx[1] - 1;
                m->tris[m->ntris - 1].idx[2] = idx[2] - 1;
            }
        }
    }

    // blender objects can't seem to get their vertex ordering consistently right.
    // if the normals are wrong just invert them all, they're all consistent within
    // a single mesh.
    Vec3f norm = mesh_tri_normal(m, m->tris[0]);

    if (vec_mulv(norm, m->pts[m->tris[0].idx[0]]) < 0.f)
        m->invert_normal = true;

    for (size_t i = 0; i < m->ntris; ++i)
        m->tris[i].norm = mesh_tri_normal(m, m->tris[i]);

    free(line);
    fclose(f);
}


bool mesh_ray_intersect(struct Mesh *m, Vec3f ro, Vec3f rdir, float *t, Triangle *tri)
{
    float nearest = INFINITY;
    *t = INFINITY;

    for (size_t i = 0; i < m->ntris; ++i)
    {
        if (m->tris[i].norm.z > 0.f)
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
    Vec3f a = m->pts[tri.idx[0]];
    a = vec_addv(a, m->pos);

    Vec3f b = m->pts[tri.idx[1]];
    b = vec_addv(b, m->pos);

    Vec3f c = m->pts[tri.idx[2]];
    c = vec_addv(c, m->pos);

    *t = (vec_mulv(a, tri.norm) - vec_mulv(ro, tri.norm)) / vec_mulv(rdir, tri.norm);

    // check if inside triangle
    Vec3f p = vec_addv(ro, vec_mulf(rdir, *t));

    Vec3f ca = vec_sub(c, a);
    Vec3f ba = vec_sub(b, a);

    float a1 = vec_mulv(ba, ba);
    float b1 = vec_mulv(ca, ba);
    float b2 = vec_mulv(ca, ca);

    float c1 = vec_mulv(ba, vec_sub(p, a));
    float c2 = vec_mulv(ca, vec_sub(p, a));

    float y = ((c1 * b1) - (c2 * a1)) / ((b1 * b1) - (a1 * b2));
    float x = (c1 - (b1 * y)) / a1;

    return (x >= 0.f && x <= 1.f && y >= 0.f && y <= 1.f && x + y >= 0.f && x + y <= 1.f && *t >= 0.f);
}


void mesh_find_bounds(struct Mesh *m, Vec3f ro)
{
    // Vec3f l = { INFINITY, 0.f, 0.f };
    // int lx = INFINITY;

    // Vec3f r = { -INFINITY, 0.f, 0.f };
    // Vec3f t = { 0.f, INFINITY, 0.f };
    // Vec3f b = { 0.f, -INFINITY, 0.f };

    Vec3f l, r, t, b;
    float lx = INFINITY;
    float rx = -INFINITY;
    float ty = INFINITY;
    float by = -INFINITY;

    for (size_t i = 0; i < m->npts; ++i)
    {
        Vec3f adjusted = vec_addv(m->pts[i], m->pos);
        SDL_Point p = util_project_point(adjusted, 1000, 1000);

        if (p.x < lx)
        {
            lx = p.x;
            l = adjusted;
        }

        if (p.x > rx)
        {
            rx = p.x;
            r = adjusted;
        }

        if (p.y < ty)
        {
            ty = p.y;
            t = adjusted;
        }

        if (p.y > by)
        {
            by = p.y;
            b = adjusted;
        }
    }

    if (t.z < ro.z)
        m->top_ry = -1.f;
    else
        m->top_ry = vec_normalize(vec_sub(t, ro)).y;

    if (b.z < ro.z)
        m->bot_ry = 1.f;
    else
        m->bot_ry = vec_normalize(vec_sub(b, ro)).y;

    if (l.z < ro.z)
        m->left_rx = -1.f;
    else
        m->left_rx = vec_normalize(vec_sub(l, ro)).x;

    if (r.z < ro.z)
        m->right_rx = 1.f;
    else
        m->right_rx = vec_normalize(vec_sub(r, ro)).x;
}


Vec3f mesh_tri_normal(struct Mesh *m, Triangle t)
{
    int i1 = 2;
    int i2 = 1;

    if (m->invert_normal)
    {
        i1 = 1;
        i2 = 2;
    }

    return vec_normalize(vec_cross(vec_sub(m->pts[t.idx[i1]], m->pts[t.idx[0]]), vec_sub(m->pts[t.idx[i2]], m->pts[t.idx[0]])));
}
