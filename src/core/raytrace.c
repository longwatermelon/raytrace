#include "raytrace.h"
#include "render.h"
#include "video.h"
#include <pthread.h>


void raytrace_image(const char *config, const char *out)
{
    struct Scene *scene = scene_alloc(config);
    Vec3f *frame = render_rend(scene);
    render_write_to_file(scene, frame, out);

    free(frame);
    scene_free(scene);
}


void raytrace_video(const char *config, const char *out)
{
    struct Video *v = video_alloc(config);
    video_create(v);
    video_free(v);
}


void *raytrace_thr_image(void *arg)
{
    raytrace_args_t *args = (raytrace_args_t*)arg;
    raytrace_image(args->config, args->out);
    pthread_exit(0);
}


void *raytrace_thr_video(void *arg)
{
    raytrace_args_t *args = (raytrace_args_t*)arg;
    raytrace_video(args->config, args->out);
    pthread_exit(0);
}

