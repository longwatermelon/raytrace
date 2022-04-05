#ifndef RAYTRACE_H
#define RAYTRACE_H

#include "render.h"

#define raytrace_loglevel(x) util_set_loglevel(x)

void raytrace_image(const char *config, const char *out);
void raytrace_video(const char *config, const char *out);

#endif

