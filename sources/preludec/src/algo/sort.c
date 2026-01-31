#include <preludec/algo/sort.h>


void insertion_sort(
  void *base,
  usize nmemb,
  usize size,
  int (*cmpfn)(void *, const void *, const void *),
  void *udata
) {
  assert(base != NULL);
  assert(nmemb > 0);
  assert(size > 0);
  assert(cmpfn != NULL);

  u8 *arr       = (u8 *)base;
  u8  key[size] = {};

  for (usize i = 1; i < nmemb; ++i) {
    u8 *pi = arr + i * size;
    memcpy(key, pi, size);

    usize j = i;
    while (j > 0) {
      u8 *pj1 = arr + (j - 1) * size;

      if (cmpfn(udata, pj1, key) <= 0) {
        break;
      }

      memcpy(arr + j * size, pj1, size);
      --j;
    }

    memcpy(arr + j * size, key, size);
  }
}
