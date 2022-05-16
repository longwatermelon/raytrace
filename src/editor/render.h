#ifndef ANIMATE_RENDER_H
#define ANIMATE_RENDER_H

#include <core/scene.h>
#include <SDL2/SDL.h>

void render_scene(struct Scene *sc, struct Mesh *selected, SDL_Renderer *rend);
void render_scene_mesh(struct Scene *sc, struct Mesh *m, bool selected, SDL_Renderer *rend);

void render_set_size(int w, int h);

#endif

