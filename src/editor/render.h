#ifndef EDITOR_RENDER_H
#define EDITOR_RENDER_H

#include <core/scene.h>
#include <SDL2/SDL.h>

struct Prog;

void render_scene(struct Prog *p);
void render_scene_mesh(struct Prog *p, struct Mesh *m);

void render_set_size(int w, int h);

#endif

