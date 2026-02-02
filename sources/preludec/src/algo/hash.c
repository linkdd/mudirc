#include <preludec/algo/hash.h>


u64 fnv1a(const_span buf) {
  assert(buf.data != NULL && buf.size > 0);

  const u64 FNV_OFFSET_BASIS = 14695981039346656037ULL;
  const u64 FNV_PRIME        = 1099511628211ULL;

  u64 hash = FNV_OFFSET_BASIS;

  for (usize i = 0; i < buf.size; ++i) {
    hash ^= ((const u8*)buf.data)[i];
    hash *= FNV_PRIME;
  }

  return hash;
}
