#pragma once

#include <math.h>

#include <preludec/defs.h>


static constexpr f32 F32_PI  = 3.141592653589793238462643383279502884f;
static constexpr f32 F32_TAU = 2.0f * F32_PI;

static constexpr f64 F64_PI  = 3.141592653589793238462643383279502884;
static constexpr f64 F64_TAU = 2.0 * F64_PI;


static inline i64 sat_add_i64_u64(i64 a, u64 b) {
  if (b > (u64)INT64_MAX) return INT64_MAX;
  i64 bi = (i64)b;
  if (a > INT64_MAX - bi) return INT64_MAX;
  return a + bi;
}


static inline i64 sat_sub_i64_u64(i64 a, u64 b) {
  if (b > (u64)INT64_MAX) return INT64_MIN;
  i64 bi = (i64)b;
  if (a < INT64_MIN + bi) return INT64_MIN;
  return a - bi;
}


static inline u64 i64_to_ordered_u64(i64 x) {
  return ((u64)x) ^ 0x8000000000000000ULL;
}
