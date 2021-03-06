#ifndef CORE_RENDER_H
#define CORE_RENDER_H

#include "scene.h"
#include "camera.h"
#include "util.h"

#include <stdbool.h>
#include <sys/types.h>
#include <SDL2/SDL.h>

typedef struct
{
    struct Scene *sc; Vec3f *frame; int y; int step;
    size_t *rows_rendered; bool *done;
} render_cast_rays_args;

Vec3f *render_rend(struct Scene *sc);

Vec3f *render_rend_cast_rays(struct Scene *sc);
void render_rend_wait_cthreads(render_cast_rays_args **args);

void render_write_to_file(struct Scene *sc, Vec3f *frame, const char *out);

void render_print_progress(struct Scene *sc, size_t rows_rendered);
void render_print_config(struct Scene *sc);

void *render_cast_rays(void *args);
// bounce: number of times ray has bounced
Vec3f render_cast_ray(struct Scene *sc, Vec3f o, Vec3f dir, Point pixel, bool optimize_meshes, int bounce);
// hit: intersection point
// n: normal
// optimize_meshes: don't calculate meshes that aren't within range when true
bool render_scene_cast_ray(struct Scene *sc, Vec3f o, Vec3f dir, Point pixel, bool optimize_meshes, Vec3f *hit, Vec3f *n, void **obj, int *obj_type);

Vec3f *render_apply_antialiasing(struct Scene *sc, Vec3f *frame);
Vec3f render_smoothen(struct Scene *sc, Vec3f *frame, int cx, int cy);

#endif

