#pragma once
#ifndef UTILS_H_
#define UTILS_H_

// Utility macros for vector operations

#define dot(v0, v1) ((v0).x * (v1).x + (v0).y * (v1).y)
#define length(v) (sqrtf(dot(v, v)))
#define normalize(v)                                                           \
  ({                                                                           \
    float len = length(v);                                                     \
    (len > 0) ? ((vec2f_t){(v).x / len, (v).y / len}) : (vec2f_t){0, 0};       \
  })
#define sign(v) ((v) < 0 ? -1 : (v) > 0 ? 1 : 0)
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

// Vector types
typedef struct {
  float x, y;
} vec2f_t;
typedef struct {
  int x, y;
} vec2i_t;

#endif // UTILS_H_
