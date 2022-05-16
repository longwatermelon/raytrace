#ifndef CORE_VIDEO_H
#define CORE_VIDEO_H

#include "scene.h"
#include <stdlib.h>

struct VideoEvent
{
    int type; // object type for casting
    void *obj; // original object
    void *delta; // change

    size_t fbegin, fend; // frame begin, end
};

struct VideoEvent *ve_alloc(struct Scene *sc, char *s);
void ve_free(struct VideoEvent *ve);

void *ve_parse_obj(struct Scene *s, char *line, int type);

struct Video
{
    struct Scene *base;

    struct VideoEvent **events;
    size_t nevents;

    size_t nframes;
};

struct Video *video_alloc(const char *config);
void video_free(struct Video *v);

void video_load_config(struct Video *v, const char *config);

void video_create(struct Video *v);
void video_apply_delta(struct Video *v, void *obj, void *delta, int type);

#endif

