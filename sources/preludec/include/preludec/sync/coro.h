#pragma once

#include <preludec/defs.h>
#include <preludec/mem/alloc.h>
#include <preludec/mem/span.h>
#include <preludec/collections/vec.h>


typedef struct coro coro;
typedef void (*coro_fn)(coro *co, allocator a, span arg);

VEC_DEFINE(coro, coro*);

typedef struct coroset coroset;
struct coroset {
  allocator a;

  VEC(coro) coros;
};


coro *make_coro(allocator a, coro_fn fn, const_span arg);
void  free_coro(coro *self);

void coro_yield(coro *self, coro *nested);

void coroset_init  (coroset *self, allocator a);
void coroset_deinit(coroset *self);

void coroset_add (coroset *self, coro *co);
void coroset_poll(coroset *self);
