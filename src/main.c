#include "render.h"


int main(int argc, char **argv)
{
    struct Sphere *spheres[3] = {
        sphere_alloc((Vec3f){ 0.f, 0.f, 5.f }, 1.f),
        sphere_alloc((Vec3f){ 1.f, 0.f, 8.f }, 2.f),
        sphere_alloc((Vec3f){ -2.f, 1.f, 6.f }, 1.f)
    };

    size_t nspheres = sizeof(spheres) / sizeof(spheres[0]);

    Light lights[3] = {
        // (Light){ (Vec3f){ 1.f, 0.f, 0.f }, 0.8f }
        (Light){ (Vec3f){ 3.f, 3.f, 5.f }, 0.8f },
        (Light){ (Vec3f){ -.5f, -.5f, 2.f }, 0.8f },
        (Light){ (Vec3f){ -.2f, -1.f, 4.2f }, 1.f }
    };

    size_t nlights = sizeof(lights) / sizeof(lights[0]);

    struct Mesh *meshes[1] = {
        mesh_alloc((Vec3f){ -.7f, -.7f, 3.f }, "cube")
    };

    size_t nmeshes = sizeof(meshes) / sizeof(meshes[0]);
    // (void)nmeshes;

    render_set_dim(1000, 1000);
    render_set_spheres(spheres, nspheres);
    render_set_lights(lights, nlights);
    render_set_meshes(meshes, nmeshes);

    render_rend();

    for (size_t i = 0; i < nspheres; ++i)
        sphere_free(spheres[i]);

    return 0;
}
