#include <preludec/mem/alloc.h>


static void *std_allocate(void *udata, usize sz) {
  (void)udata;
  return malloc(sz);
}


static void *std_reallocate(void *udata, void *ptr, usize oldsz, usize newsz) {
  (void)udata;
  (void)oldsz;
  return realloc(ptr, newsz);
}


static void std_deallocate(void *udata, void *ptr, usize sz) {
  (void)udata;
  (void)sz;
  free(ptr);
}


allocator std_allocator(void) {
  return (allocator){
    .allocate   = std_allocate,
    .reallocate = std_reallocate,
    .deallocate = std_deallocate,
    .udata      = NULL,
  };
}


void *allocate(allocator a, usize sz) {
  assert(a.allocate != NULL);

  void *ptr = a.allocate(a.udata, sz);
  assert_release(ptr != NULL);
  memset(ptr, 0, sz);

  return ptr;
}


void *allocate_v(allocator a, usize nmemb, usize membsz) {
  assert(a.allocate != NULL);

  assert_release(SIZE_MAX / nmemb >= membsz);
  usize sz = nmemb * membsz;
  return allocate(a, sz);
}


void *reallocate(allocator a, void *ptr, usize oldsz, usize newsz) {
  assert(a.reallocate != NULL);

  void *newptr = a.reallocate(a.udata, ptr, oldsz, newsz);
  if (newptr != NULL && newsz > oldsz) {
    memset((u8 *)newptr + oldsz, 0, newsz - oldsz);
  }

  return newptr;
}


void deallocate(allocator a, void *ptr, usize sz) {
  assert(a.deallocate != NULL);

  if (ptr != NULL) {
    a.deallocate(a.udata, ptr, sz);
  }
}
