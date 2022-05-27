#ifndef EDITOR_WRITER_H
#define EDITOR_WRITER_H

#include "config.h"
#include <core/scene.h>

void writer_image(struct Scene *sc, struct Config *c, const char *fp);
void writer_image_mats(struct Scene *sc, char **out);
void writer_image_meshes(struct Scene *sc, char **out);
void writer_image_spheres(struct Scene *sc, char **out);
void writer_image_lights(struct Scene *sc, char **out);

#endif

