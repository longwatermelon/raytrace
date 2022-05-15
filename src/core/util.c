#include "util.h"

int g_loglevel = LOG_NORMAL;

void util_set_loglevel(int level) { g_loglevel = level; }
int util_loglevel() { return g_loglevel; }

Vec3f util_matmul(float mat[3][3], Vec3f p)
{
    Vec3f ret;

    ret.x = p.x * mat[0][0] + p.y * mat[1][0] + p.z * mat[2][0];
    ret.y = p.x * mat[0][1] + p.y * mat[1][1] + p.z * mat[2][1];
    ret.z = p.x * mat[0][2] + p.y * mat[1][2] + p.z * mat[2][2];

    return ret;
}

