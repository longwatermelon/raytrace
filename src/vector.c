#include "vector.h"
#include <math.h>


Vec3f vec_normalize(Vec3f v)
{
    float len = vec_len(v);

    return (Vec3f){
        .x = v.x / len,
        .y = v.y / len,
        .z = v.z / len
    };
}


float vec_len(Vec3f v)
{
    return sqrtf(
        v.x * v.x +
        v.y * v.y +
        v.z * v.z
    );
}


Vec3f vec_addv(Vec3f v1, Vec3f v2)
{
    return (Vec3f){
        .x = v1.x + v2.x,
        .y = v1.y + v2.y,
        .z = v1.z + v2.z
    };
}


Vec3f vec_addf(Vec3f v, float f)
{
    return (Vec3f){
        .x = v.x + f,
        .y = v.y + f,
        .z = v.z + f
    };
}


Vec3f vec_sub(Vec3f v1, Vec3f v2)
{
    return (Vec3f){
        .x = v1.x - v2.x,
        .y = v1.y - v2.y,
        .z = v1.z - v2.z
    };
}


Vec3f vec_neg(Vec3f v)
{
    return (Vec3f){
        .x = -v.x,
        .y = -v.y,
        .z = -v.z
    };
}


float vec_mulv(Vec3f v1, Vec3f v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}


Vec3f vec_mulf(Vec3f v, float f)
{
    return (Vec3f){
        .x = v.x * f,
        .y = v.y * f,
        .z = v.z * f
    };
}
