#pragma once

#include <preludec/defs.h>
#include <preludec/mem/alloc.h>
#include <preludec/sync/coro.h>
#include <preludec/chrono/time.h>


coro *wait_for(allocator a, duration dur);
