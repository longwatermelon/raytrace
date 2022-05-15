#include "util.h"

int g_loglevel = LOG_NORMAL;

void util_set_loglevel(int level) { g_loglevel = level; }
int util_loglevel() { return g_loglevel; }

