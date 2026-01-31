#pragma once

#include <preludec/defs.h>
#include <preludec/mem/alloc.h>
#include <preludec/mem/span.h>


typedef struct arena arena;
struct arena {
  span  memory;
  usize offset;
};


void arena_init (arena *a, span memory);
void arena_reset(arena *a);

allocator arena_allocator(arena *a);
