#pragma once

#include <preludec/defs.h>


typedef void  (*lc_shutdown_request)(void *ctx);


void lc_init(void *ctx, lc_shutdown_request shutdown_request_fn);

bool lc_running(void);
