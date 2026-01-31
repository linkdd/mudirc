#include <time.h>


static inline i64 timespec_to_ns(struct timespec ts) {
  assert((u64)ts.tv_sec <= (u64)INT64_MAX / 1000000000ULL);
  return (i64)ts.tv_sec * 1000000000LL + (i64)ts.tv_nsec;
}


timepoint system_clock_now(void) {
  struct timespec ts = {};

  assert_release(0 == clock_gettime(CLOCK_REALTIME, &ts));

  return (timepoint){ .ns = timespec_to_ns(ts) };
}


timepoint steady_clock_now(void) {
  struct timespec ts = {};

  assert_release(0 == clock_gettime(CLOCK_MONOTONIC, &ts));

  return (timepoint){ .ns = timespec_to_ns(ts) };
}
