#pragma once

#include <preludec/defs.h>
#include <preludec/mem/alloc.h>
#include <preludec/mem/span.h>


typedef struct str str;
struct str {
  char  *data;
  usize  length;
  usize  capacity;
  bool   owned;
};


#define str_literal(s)  (str){ .data = ("" s), .length = sizeof(s) - 1, .capacity = sizeof(s) - 1, .owned = false }
#define str_null()      (str){ .data = NULL,   .length = 0,             .capacity = 0,             .owned = false }


str str_make         (allocator a, usize capacity);
str strview_from_cstr(const char *cstr);
str str_from_cstr    (allocator a, const char *cstr);
str str_clone        (allocator a, str s);

void str_free(allocator a, str *s);

char *str_to_cstr(allocator a, str s);
str   str_join   (allocator a, str s1, str s2);
str   str_slice  (str s, usize start, usize end);
bool  str_equal  (str s1, str s2);

const_span str_as_const_span(str s);
span       str_as_span      (str s);
