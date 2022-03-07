#include "render.h"


int main(int argc, char **argv)
{
    struct Sphere **spheres = malloc(sizeof(struct Sphere));
    size_t nspheres = 1;

    spheres[0] = sphere_alloc((Vec3f){ 0.f, 0.f, 5.f }, 1.f);

    render_set_dim(1000, 1000);
    render_set_spheres(spheres, nspheres);

    render_rend();

    return 0;
}
