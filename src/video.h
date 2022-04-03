#ifndef VIDEO_H
#define VIDEO_H

#include "scene.h"
#include <stdlib.h>

struct VideoEvent
{
    enum { VE_SPHERE, VE_MESH } type; // object type for casting
    void *obj; // original object
    void *delta; // change

    size_t fbegin, fcount; // frame begin and frame number for entire animation
};

struct VideoEvent *ve_alloc(int type, void *obj, void *delta, size_t fbegin, size_t fcount);
void ve_free(struct VideoEvent *ve);

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
void *video_parse_obj(struct Video *v, char *line, int type);

void video_create(struct Video *v);
void video_apply_delta(void *obj, void *delta, int type);

#endif

