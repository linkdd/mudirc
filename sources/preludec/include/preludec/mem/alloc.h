#pragma once

#include <preludec/defs.h>


typedef struct allocator allocator;
struct allocator {
  void *(*allocate)  (void *udata, usize sz);
  void *(*reallocate)(void *udata, void *ptr, usize oldsz, usize newsz);
  void  (*deallocate)(void *udata, void *ptr, usize sz);

  void *udata;
};


allocator std_allocator(void);

void *allocate  (allocator a, usize sz);
void *allocate_v(allocator a, usize nmemb, usize membsz);
void *reallocate(allocator a, void *ptr, usize oldsz, usize newsz);
void  deallocate(allocator a, void *ptr, usize sz);
