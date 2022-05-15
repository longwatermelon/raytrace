#ifndef CORE_UTIL_H
#define CORE_UTIL_H

#include "vector.h"
#include <SDL2/SDL.h>

#define LOG(x, ...) { if (x <= util_loglevel()) printf(__VA_ARGS__); }

enum { LOG_SILENT, LOG_NORMAL, LOG_VERBOSE };

enum
{
    OPT_BACKFACE_CULLING = 1
};

void util_set_loglevel(int level);
int util_loglevel();

#endif
