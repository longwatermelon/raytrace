#ifndef CORE_RAYTRACE_H
#define CORE_RAYTRACE_H

#include "scene.h"

#define raytrace_loglevel(x) util_set_loglevel(x)

typedef struct { const char *config; const char *out; } raytrace_args_t;
void raytrace_image(const char *config, const char *out);
void raytrace_video(const char *config, const char *out);

void *raytrace_thr_image(void *arg);
void *raytrace_thr_video(void *arg);

typedef struct { struct Scene *sc; const char *out; bool done; } raytrace_sc_args_t;
void *raytrace_thr_sc_image(void *arg);

#endif

