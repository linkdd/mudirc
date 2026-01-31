#include <preludec/mem/str.h>


str str_make(allocator a, usize capacity) {
  assert(capacity > 0);

  return (str){
    .data     = allocate_v(a, capacity, sizeof(char)),
    .length   = 0,
    .capacity = capacity,
    .owned    = true,
  };
}


str strview_from_cstr(const char *cstr) {
  assert(cstr != NULL);

  usize len = strlen(cstr);
  return (str){
    .data     = (char *)cstr,
    .length   = len,
    .capacity = len,
    .owned    = false,
  };
}


str str_from_cstr(allocator a, const char *cstr) {
  assert(cstr != NULL);

  usize  len  = strlen(cstr);
  char  *data = allocate_v(a, len, sizeof(char));
  memcpy(data, cstr, len);

  return (str){
    .data     = data,
    .length   = len,
    .capacity = len,
    .owned    = true,
  };
}


str str_clone(allocator a, str s) {
  if (s.length == 0) {
    return str_null();
  }

  char *data = allocate_v(a, s.length, sizeof(char));
  memcpy(data, s.data, s.length);

  return (str){
    .data     = data,
    .length   = s.length,
    .capacity = s.length,
    .owned    = true,
  };
}


void str_free(allocator a, str *s) {
  assert(s != NULL);

  if (s->owned && s->data != NULL) {
    deallocate(a, s->data, s->capacity * sizeof(char));
  }

  s->data     = NULL;
  s->length   = 0;
  s->capacity = 0;
  s->owned    = false;
}


char *str_to_cstr(allocator a, str s) {
  char *cstr = allocate_v(a, s.length + 1, sizeof(char));
  memcpy(cstr, s.data, s.length);
  cstr[s.length] = '\0';

  return cstr;
}


str str_join(allocator a, str s1, str s2) {
  usize  total_len = s1.length + s2.length;
  char  *data      = allocate_v(a, total_len, sizeof(char));

  memcpy(data,             s1.data, s1.length);
  memcpy(data + s1.length, s2.data, s2.length);

  return (str){
    .data     = data,
    .length   = total_len,
    .capacity = total_len,
    .owned    = true,
  };
}


str str_slice(str s, usize start, usize end) {
  if (s.data == NULL) {
    return str_null();
  }

  if (start > s.length) start = s.length;
  if (end   > s.length) end   = s.length;
  if (start > end)      start = end;

  return (str){
    .data     = s.data + start,
    .length   = end - start,
    .capacity = s.capacity - start,
    .owned    = false,
  };
}


bool str_equal(str s1, str s2) {
  if (s1.length != s2.length) {
    return false;
  }

  return memcmp(s1.data, s2.data, s1.length) == 0;
}


const_span str_as_const_span(str s) {
  return make_const_span(s.data, s.capacity * sizeof(char));
}


span str_as_span(str s) {
  return make_span(s.data, s.capacity * sizeof(char));
}
