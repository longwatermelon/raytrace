#ifndef CORE_CAMERA_H
#define CORE_CAMERA_H

#include "util.h"

struct Camera
{
    Vec3f pos;
    Vec3f angle;
};

struct Camera *cam_alloc(Vec3f pos, Vec3f angle);
void cam_free(struct Camera *c);

#endif

