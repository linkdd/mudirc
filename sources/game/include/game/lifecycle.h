#pragma once

#include <preludec/defs.h>


void lc_init   (void *ctx, void (*destructor)(void *));
bool lc_running(void);
