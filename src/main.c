#include "render.h"
#include <math.h>


int main(int argc, char **argv)
{
    struct Sphere *spheres[] = {
        sphere_alloc((Vec3f){ -3.f, 3.f, 9.f }, 1.f, (Vec3f){ 1.f, .95f, .7f }),
        sphere_alloc((Vec3f){ 2.f, 2.f, 20.f }, 2.f, (Vec3f){ 1.f, .5f, 1.f })
    };

    size_t nspheres = sizeof(spheres) / sizeof(spheres[0]);

    Light lights[] = {
        (Light){ (Vec3f){ 4.f, -1.f, 0.f }, 0.8f },
        (Light){ (Vec3f){ -3.f, -5.f, 6.f }, 0.8f }
    };

    size_t nlights = sizeof(lights) / sizeof(lights[0]);

    struct Mesh *meshes[] = {
        mesh_alloc((Vec3f){ 0.f, 4.f, 100.f }, "res/ground.obj", (Vec3f){ .5f, .9f, .5f }),
        mesh_alloc((Vec3f){ -3.f, 3.f, 15.f }, "res/untitled.obj", (Vec3f){ .7f, .95f, 1.f }),
        mesh_alloc((Vec3f){ 2.f, 3.f, 12.f }, "res/icosphere.obj", (Vec3f){ .9f, .6f, .7f }),
        mesh_alloc((Vec3f){ -1.f, 2.2f, 18.f }, "res/donut.obj", (Vec3f){ .6f, .7f, .9f }),
        mesh_alloc((Vec3f){ -.4f, 3.4f, 7.f }, "res/monkey.obj", (Vec3f){ .95f, .7f, .95f })
    };

    meshes[0]->left_rx = -1.f;
    meshes[0]->right_rx = 1.f;
    meshes[0]->bot_ry = 1.f;
    meshes[0]->top_ry = -1.f;

    size_t nmeshes = sizeof(meshes) / sizeof(meshes[0]);

    render_set_dim(1000, 1000);
    render_set_spheres(spheres, nspheres);
    render_set_lights(lights, nlights);
    render_set_meshes(meshes, nmeshes);

    render_rend();

    for (size_t i = 0; i < nspheres; ++i)
        sphere_free(spheres[i]);

    for (size_t i = 0; i < nmeshes; ++i)
        mesh_free(meshes[i]);

    return 0;
}
