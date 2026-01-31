#define MINICORO_IMPL
#define MCO_NO_DEFAULT_ALLOCATOR  1
#define MCO_ZERO_MEMORY           1

#ifdef DEBUG
#define MCO_DEBUG                 1
#else
#define MCO_NO_DEBUG              1
#endif

#include <minicoro.h>

#include <preludec/sync/coro.h>


VEC_IMPLEMENTATION(coro, coro*);

struct coro {
  allocator a;
  coro_fn   fn;
  span      arg;

  mco_desc desc;
  mco_coro *co;

  coro *parent;
};


static void coro_task(mco_coro *co) {
  coro *self = mco_get_user_data(co);
  assert(self != NULL);

  self->fn(self, self->a, self->arg);
}


static void coro_init(coro *self, allocator a, coro_fn fn, const_span arg) {
  assert(self != NULL);
  assert(fn   != NULL);

  self->a   = a;
  self->fn  = fn;
  self->arg = (arg.data != NULL ? span_clone(a, arg) : span_null());

  self->desc           = mco_desc_init(coro_task, 0);
  self->desc.user_data = self;

  self->co = allocate(a, self->desc.coro_size);
  assert_release(MCO_SUCCESS == mco_init(self->co, &self->desc));

  self->parent = NULL;
}


static void coro_deinit(coro *self) {
  assert(self != NULL);

  if (self->co != NULL) {
    assert_release(MCO_SUCCESS == mco_uninit(self->co));
    deallocate(self->a, self->co, self->desc.coro_size);
  }

  memset(self, 0, sizeof(coro));
}


coro *make_coro(allocator a, coro_fn fn, const_span arg) {
  assert(fn != NULL);

  coro *c = allocate(a, sizeof(coro));
  coro_init(c, a, fn, arg);

  return c;
}


void free_coro(coro *self) {
  assert(self != NULL);

  coro_deinit(self);
  deallocate(self->a, self, sizeof(coro));
  memset(self, 0, sizeof(coro));
}


void coro_yield(coro *self, coro *nested) {
  assert(self != NULL);

  assert_release(MCO_SUCCESS == mco_push(self->co, &nested, sizeof(coro*)));
  assert_release(MCO_SUCCESS == mco_yield(self->co));
}


void coroset_init(coroset *self, allocator a) {
  assert(self != NULL);

  self->a = a;
  vec_coro_init(&self->coros, a, 4);
}


void coroset_deinit(coroset *self) {
  assert(self != NULL);

  for (usize i = 0; i < self->coros.count; ++i) {
    coro *c = *vec_coro_at(&self->coros, i);
    free_coro(c);
  }
  vec_coro_deinit(&self->coros);

  memset(self, 0, sizeof(coroset));
}


void coroset_add(coroset *self, coro *co) {
  assert(self != NULL);
  assert(co   != NULL);

  vec_coro_push(&self->coros, co);
}


void coroset_poll(coroset *self) {
  assert(self != NULL);

  for (usize i = 0; i < self->coros.count; ++i) {
    coro **c = vec_coro_at(&self->coros, i);

    assert_release(MCO_SUCCESS == mco_resume((*c)->co));

    switch (mco_status((*c)->co)) {
      case MCO_SUSPENDED: {
        coro *nested = NULL;

        assert_release(MCO_SUCCESS == mco_pop((*c)->co, &nested, sizeof(coro*)));
        if (nested != NULL) {
          nested->parent = *c;
          *c             = nested;
        }

        break;
      }

      case MCO_DEAD: {
        coro *old = *c;
        *c        = old->parent;

        free_coro(old);
        if (*c == NULL) {
          vec_coro_erase(&self->coros, i);
          i--;
        }

        break;
      }

      default:
        break;
    }
  }
}
