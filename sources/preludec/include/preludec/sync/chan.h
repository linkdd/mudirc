#pragma once

#include <preludec/defs.h>
#include <preludec/mem/alloc.h>
#include <preludec/sync/coro.h>


#define CHAN(name)  chan_##name

#define CHAN_DEFINE(name, T)                                                   \
  typedef struct CHAN(name) CHAN(name);                                        \
  struct CHAN(name) {                                                          \
    allocator a;                                                               \
                                                                               \
    struct {                                                                   \
      T     *items;                                                            \
      usize  capacity;                                                         \
      usize  count;                                                            \
      usize  first;                                                            \
      usize  last;                                                             \
    } buffer;                                                                  \
  };                                                                           \
                                                                               \
  void chan_##name##_init  (CHAN(name) *self, allocator a, usize capacity);    \
  void chan_##name##_deinit(CHAN(name) *self);                                 \
                                                                               \
  coro *chan_##name##_send   (CHAN(name) *self, allocator a, T  item);         \
  coro *chan_##name##_receive(CHAN(name) *self, allocator a, T *dest);


#define CHAN_IMPLEMENTATION(name, T)                                           \
  static inline bool chan_##name##_is_full(CHAN(name) *self) {                 \
    return self->buffer.count == self->buffer.capacity;                        \
  }                                                                            \
                                                                               \
  static inline bool chan_##name##_is_empty(CHAN(name) *self) {                \
    return self->buffer.count == 0;                                            \
  }                                                                            \
                                                                               \
  static void chan_##name##_enqueue(CHAN(name) *self, T item) {                \
    assert(!chan_##name##_is_full(self));                                      \
                                                                               \
    memcpy(                                                                    \
      &self->buffer.items[self->buffer.last],                                  \
      &item,                                                                   \
      sizeof(T)                                                                \
    );                                                                         \
                                                                               \
    self->buffer.last = (self->buffer.last + 1) % self->buffer.capacity;       \
    self->buffer.count++;                                                      \
  }                                                                            \
                                                                               \
  static void chan_##name##_dequeue(CHAN(name) *self, T *dest) {               \
    assert(!chan_##name##_is_empty(self));                                     \
                                                                               \
    memcpy(                                                                    \
      dest,                                                                    \
      &self->buffer.items[self->buffer.first],                                 \
      sizeof(T)                                                                \
    );                                                                         \
                                                                               \
    self->buffer.first = (self->buffer.first + 1) % self->buffer.capacity;     \
    self->buffer.count--;                                                      \
  }                                                                            \
                                                                               \
  void chan_##name##_init(CHAN(name) *self, allocator a, usize capacity) {     \
    assert(self != NULL);                                                      \
    assert(capacity > 0);                                                      \
                                                                               \
    self->a = a;                                                               \
    self->buffer.items    = allocate_v(a, capacity, sizeof(T));                \
    self->buffer.capacity = capacity;                                          \
    self->buffer.count    = 0;                                                 \
    self->buffer.first    = 0;                                                 \
    self->buffer.last     = 0;                                                 \
  }                                                                            \
                                                                               \
  void chan_##name##_deinit(CHAN(name) *self) {                                \
    assert(self != NULL);                                                      \
                                                                               \
    if (self->buffer.items != NULL) {                                          \
      deallocate(                                                              \
        self->a,                                                               \
        self->buffer.items,                                                    \
        sizeof(T) * self->buffer.capacity                                      \
      );                                                                       \
    }                                                                          \
                                                                               \
    memset(self, 0, sizeof(CHAN(name)));                                       \
  }                                                                            \
                                                                               \
  struct chan_##name##_send_task_ctx {                                         \
    CHAN(name) *chan;                                                          \
    T           item;                                                          \
  };                                                                           \
                                                                               \
  static void chan_##name##_send_task(coro *co, allocator a, span arg) {       \
    struct chan_##name##_send_task_ctx *ctx = arg.data;                        \
                                                                               \
    while (chan_##name##_is_full(ctx->chan)) coro_yield(co, NULL);             \
    chan_##name##_enqueue(ctx->chan, ctx->item);                               \
  }                                                                            \
                                                                               \
  coro *chan_##name##_send(CHAN(name) *self, allocator a, T item) {            \
    assert(self != NULL);                                                      \
                                                                               \
    struct chan_##name##_send_task_ctx ctx = { .chan = self, .item = item };   \
                                                                               \
    return make_coro(                                                          \
      a,                                                                       \
      chan_##name##_send_task,                                                 \
      make_const_span(&ctx, sizeof(ctx))                                       \
    );                                                                         \
  }                                                                            \
                                                                               \
  struct chan_##name##_receive_task_ctx {                                      \
    CHAN(name) *chan;                                                          \
    T          *dest;                                                          \
  };                                                                           \
                                                                               \
  static void chan_##name##_receive_task(coro *co, allocator a, span arg) {    \
    struct chan_##name##_receive_task_ctx *ctx = arg.data;                     \
                                                                               \
    while (chan_##name##_is_empty(ctx->chan)) coro_yield(co, NULL);            \
    chan_##name##_dequeue(ctx->chan, ctx->dest);                               \
  }                                                                            \
                                                                               \
  coro *chan_##name##_receive(CHAN(name) *self, allocator a, T *dest) {        \
    assert(self != NULL);                                                      \
                                                                               \
    struct chan_##name##_receive_task_ctx ctx = { .chan = self, .dest = dest };\
                                                                               \
    return make_coro(                                                          \
      a,                                                                       \
      chan_##name##_receive_task,                                              \
      make_const_span(&ctx, sizeof(ctx))                                       \
    );                                                                         \
  }
