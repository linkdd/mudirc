#include <preludec/chrono/time.h>
#include <preludec/math.h>


timepoint timepoint_add(timepoint tp, duration dur) {
  return (timepoint){ .ns = sat_add_i64_u64(tp.ns, dur.ns) };
}


timepoint timepoint_sub(timepoint tp, duration dur) {
  return (timepoint){ .ns = sat_sub_i64_u64(tp.ns, dur.ns) };
}


timespan make_timespan(timepoint start, timepoint end) {
  assert(end.ns >= start.ns);
  return (timespan){ .start = start, .end = end };
}


duration timespan_duration(timespan span) {
  assert(span.end.ns >= span.start.ns);

  u64 uend   = i64_to_ordered_u64(span.end.ns);
  u64 ustart = i64_to_ordered_u64(span.start.ns);

  return (duration){ .ns = uend - ustart };
}


duration make_duration(f64 seconds) {
  assert(!isnan(seconds) && !isinf(seconds));
  assert(seconds >= 0.0);
  assert(seconds <= ((f64)UINT64_MAX / 1e9));

  return (duration){ .ns = (u64)(seconds * 1e9) };
}


#ifdef _WIN32
# include "time_win32.inc"
#else
# include "time_posix.inc"
#endif
