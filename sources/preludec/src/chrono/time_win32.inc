#define WIN32_LEAN_AND_MEAN
#include <windows.h>


static inline i64 filetime_to_ns(FILETIME ft) {
  ULARGE_INTEGER u;
  u.LowPart  = ft.dwLowDateTime;
  u.HighPart = ft.dwHighDateTime;
  return (i64)(u.QuadPart * 100);
}


timepoint system_clock_now(void) {
  FILETIME ft;
  GetSystemTimePreciseAsFileTime(&ft);
  return (timepoint){ .ns = filetime_to_ns(ft) };
}


timepoint steady_clock_now(void) {
  static LARGE_INTEGER freq = {0};
  LARGE_INTEGER counter;

  if (freq.QuadPart == 0) {
    QueryPerformanceFrequency(&freq);
  }

  QueryPerformanceCounter(&counter);

  return (timepoint){
    .ns = (i64)((counter.QuadPart * 1000000000LL) / freq.QuadPart)
  };
}
