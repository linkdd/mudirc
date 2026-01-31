#pragma once

#include <preludec/defs.h>


typedef struct timepoint timepoint;
struct timepoint {
  i64 ns;
};

typedef struct timespan timespan;
struct timespan {
  timepoint start;
  timepoint end;
};

typedef struct duration duration;
struct duration {
  u64 ns;
};


timepoint timepoint_add(timepoint tp, duration dur);
timepoint timepoint_sub(timepoint tp, duration dur);

timespan make_timespan    (timepoint start, timepoint end);
duration timespan_duration(timespan span);

duration make_duration(f64 seconds);

timepoint system_clock_now(void);
timepoint steady_clock_now(void);
