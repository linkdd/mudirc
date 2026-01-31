#pragma once

#include <preludec/defs.h>


static inline usize align_size_up(usize size, usize alignment) {
  return (size + alignment - 1) & ~(alignment - 1);
}


static inline usize align_size_down(usize size, usize alignment) {
  return size & ~(alignment - 1);
}


static inline uptr align_ptr_up(uptr ptr, usize alignment) {
  return (ptr + alignment - 1) & ~(alignment - 1);
}


static inline uptr align_ptr_down(uptr ptr, usize alignment) {
  return ptr & ~(alignment - 1);
}
