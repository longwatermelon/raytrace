#include "render.h"


int main(int argc, char **argv)
{
    struct Sphere *spheres[1] = {
        sphere_alloc((Vec3f){ 0.f, 0.f, 5.f }, 1.f)
    };

    size_t nspheres = sizeof(spheres) / sizeof(spheres[0]);

    Light lights[1] = {
        (Light){ (Vec3f){ 3.f, 3.f, 5.f }, 0.8f }
    };

    size_t nlights = sizeof(lights) / sizeof(lights[0]);

    render_set_dim(1000, 1000);
    render_set_spheres(spheres, nspheres);
    render_set_lights(lights, nlights);

    render_rend();

    for (size_t i = 0; i < nspheres; ++i)
        sphere_free(spheres[i]);

    return 0;
}
