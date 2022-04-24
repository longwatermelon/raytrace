#include "raytrace.h"
#include "render.h"
#include "video.h"


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

