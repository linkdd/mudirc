#pragma once

#include <preludec/defs.h>
#include <preludec/mem/alloc.h>
#include <preludec/mem/span.h>


#define VEC(name)  vec_##name


#define VEC_DEFINE(name, T)                                                    \
  typedef struct VEC(name) VEC(name);                                          \
  struct VEC(name) {                                                           \
    allocator  a;                                                              \
    T         *data;                                                           \
    usize      capacity;                                                       \
    usize      count;                                                          \
  };                                                                           \
                                                                               \
  void vec_##name##_init  (VEC(name) *self, allocator a, usize capacity);      \
  void vec_##name##_deinit(VEC(name) *self);                                   \
                                                                               \
  void vec_##name##_push  (VEC(name) *self, T value);                          \
  void vec_##name##_pop   (VEC(name) *self);                                   \
  void vec_##name##_insert(VEC(name) *self, usize index, T value);             \
  void vec_##name##_erase (VEC(name) *self, usize index);                      \
                                                                               \
  void vec_##name##_clear(VEC(name) *self);                                    \
                                                                               \
  bool vec_##name##_empty(VEC(name) *self);                                    \
                                                                               \
  T *vec_##name##_at   (VEC(name) *self, usize index);                         \
  T *vec_##name##_back (VEC(name) *self);                                      \
  T *vec_##name##_front(VEC(name) *self);                                      \
                                                                               \
  void vec_##name##_resize (VEC(name) *self, usize new_size);                  \
  void vec_##name##_reserve(VEC(name) *self, usize new_capacity);              \
  void vec_##name##_shrink (VEC(name) *self);                                  \
                                                                               \
  const_span vec_##name##_as_const_span(VEC(name) *self);                      \
  span       vec_##name##_as_span      (VEC(name) *self);


#define VEC_IMPLEMENTATION(name, T)                                            \
  void vec_##name##_init(VEC(name) *self, allocator a, usize capacity) {       \
    self->a        = a;                                                        \
    self->data     = NULL;                                                     \
    self->capacity = 0;                                                        \
    self->count    = 0;                                                        \
                                                                               \
    vec_##name##_reserve(self, capacity);                                      \
  }                                                                            \
                                                                               \
  void vec_##name##_deinit(VEC(name) *self) {                                  \
    if (self->data != NULL) {                                                  \
      deallocate(self->a, self->data, sizeof(T) * self->capacity);             \
    }                                                                          \
    memset(self, 0, sizeof(VEC(name)));                                        \
  }                                                                            \
                                                                               \
  void vec_##name##_push(VEC(name) *self, T value) {                           \
    if (self->count >= self->capacity) {                                       \
      usize new_capacity = (self->capacity == 0) ? 1 : self->capacity * 2;     \
      vec_##name##_reserve(self, new_capacity);                                \
    }                                                                          \
                                                                               \
    memcpy(&self->data[self->count], &value, sizeof(T));                       \
    self->count++;                                                             \
  }                                                                            \
                                                                               \
  void vec_##name##_pop(VEC(name) *self) {                                     \
    if (self->count > 0) {                                                     \
      self->count--;                                                           \
    }                                                                          \
  }                                                                            \
                                                                               \
  void vec_##name##_insert(VEC(name) *self, usize index, T value) {            \
    assert_release(index <= self->count);                                      \
                                                                               \
    if (self->count >= self->capacity) {                                       \
      usize new_capacity = (self->capacity == 0) ? 1 : self->capacity * 2;     \
      vec_##name##_reserve(self, new_capacity);                                \
    }                                                                          \
                                                                               \
    if (index < self->count - 1) {                                             \
      memmove(                                                                 \
        &self->data[index + 1],                                                \
        &self->data[index],                                                    \
        sizeof(T) * (self->count - index)                                      \
      );                                                                       \
    }                                                                          \
                                                                               \
    memcpy(&self->data[index], &value, sizeof(T));                             \
    self->count++;                                                             \
  }                                                                            \
                                                                               \
  void vec_##name##_erase(VEC(name) *self, usize index) {                      \
    assert_release(index < self->count);                                       \
                                                                               \
    if (index < self->count - 1) {                                             \
      memmove(                                                                 \
        &self->data[index],                                                    \
        &self->data[index + 1],                                                \
        sizeof(T) * (self->count - index - 1)                                  \
      );                                                                       \
    }                                                                          \
                                                                               \
    self->count--;                                                             \
  }                                                                            \
                                                                               \
  void vec_##name##_clear(VEC(name) *self) {                                   \
    self->count = 0;                                                           \
  }                                                                            \
                                                                               \
  bool vec_##name##_empty(VEC(name) *self) {                                   \
    return self->count == 0;                                                   \
  }                                                                            \
                                                                               \
  T *vec_##name##_at(VEC(name) *self, usize index) {                           \
    if (index >= self->count) {                                                \
      return NULL;                                                             \
    }                                                                          \
                                                                               \
    return &self->data[index];                                                 \
  }                                                                            \
                                                                               \
  T *vec_##name##_back(VEC(name) *self) {                                      \
    if (self->count == 0) {                                                    \
      return NULL;                                                             \
    }                                                                          \
                                                                               \
    return &self->data[self->count - 1];                                       \
  }                                                                            \
                                                                               \
  T *vec_##name##_front(VEC(name) *self) {                                     \
    if (self->count == 0) {                                                    \
      return NULL;                                                             \
    }                                                                          \
                                                                               \
    return &self->data[0];                                                     \
  }                                                                            \
                                                                               \
  void vec_##name##_resize(VEC(name) *self, usize new_size) {                  \
    if (new_size > self->capacity) {                                           \
      vec_##name##_reserve(self, new_size);                                    \
    }                                                                          \
                                                                               \
    self->count = new_size;                                                    \
  }                                                                            \
                                                                               \
  void vec_##name##_reserve(VEC(name) *self, usize new_capacity) {             \
    if (new_capacity <= self->capacity) {                                      \
      return;                                                                  \
    }                                                                          \
                                                                               \
    T *new_data = reallocate(                                                  \
      self->a,                                                                 \
      self->data,                                                              \
      sizeof(T) * self->capacity,                                              \
      sizeof(T) * new_capacity                                                 \
    );                                                                         \
                                                                               \
    self->data     = new_data;                                                 \
    self->capacity = new_capacity;                                             \
  }                                                                            \
                                                                               \
  void vec_##name##_shrink(VEC(name) *self) {                                  \
    if (self->count == self->capacity) {                                       \
      return;                                                                  \
    }                                                                          \
                                                                               \
    T *new_data = reallocate(                                                  \
      self->a,                                                                 \
      self->data,                                                              \
      sizeof(T) * self->capacity,                                              \
      sizeof(T) * self->count                                                  \
    );                                                                         \
                                                                               \
    self->data     = new_data;                                                 \
    self->capacity = self->count;                                              \
  }                                                                            \
                                                                               \
  const_span vec_##name##_as_const_span(VEC(name) *self) {                     \
    return make_const_span(self->data, self->capacity * sizeof(T));            \
  }                                                                            \
                                                                               \
  span vec_##name##_as_span(VEC(name) *self) {                                 \
    return make_span(self->data, self->capacity * sizeof(T));                  \
  }
