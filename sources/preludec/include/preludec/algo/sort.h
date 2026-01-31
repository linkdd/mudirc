#pragma once

#include <preludec/defs.h>


void insertion_sort(
  void *base,
  usize nmemb,
  usize size,
  int (*cmpfn)(void *, const void *, const void *),
  void *udata
);
