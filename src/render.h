#ifndef RENDER_H
#define RENDER_H

#include "scene.h"

#include <stdbool.h>
#include <sys/types.h>
#include <SDL2/SDL.h>

void render_rend();
void render_print_progress();
void render_print_config();

typedef struct { Vec3f *frame; int y; int step; } render_cast_rays_args;
void *render_cast_rays(void *args);
// bounce: number of times ray has bounced
Vec3f render_cast_ray(Vec3f o, Vec3f dir, bool optimize_meshes, int bounce);
// hit: intersection point
// n: normal
// optimize_meshes: don't calculate meshes that aren't within range when true
bool render_scene_cast_ray(Vec3f o, Vec3f dir, bool optimize_meshes, Vec3f *hit, Vec3f *n, Material *mat);

Vec3f *render_apply_antialiasing(Vec3f *frame);
Vec3f render_smoothen(Vec3f *frame, int cx, int cy);

void render_set_scene(struct Scene *s);
void render_set_dim(int x, int y);
void render_set_bg(Vec3f col);
void render_set_max_bounces(int i);

void render_enable_antialiasing();
void render_set_threads(int threads);

void render_enable_optimizations(Uint32 flag);

#endif

