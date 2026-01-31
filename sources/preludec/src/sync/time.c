#include <preludec/sync/time.h>


static void wait_for_task(coro *self, allocator a, span arg) {
  (void)a;

  timepoint now = steady_clock_now();
  duration  dur = {};

  memcpy(&dur, arg.data, sizeof(duration));

  timepoint deadline = timepoint_add(now, dur);

  while (now.ns < deadline.ns) {
    coro_yield(self, NULL);
    now = steady_clock_now();
  }
}

coro *wait_for(allocator a, duration dur) {
  return make_coro(a, wait_for_task, make_const_span(&dur, sizeof(duration)));
}
