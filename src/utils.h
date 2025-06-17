#pragma once
#ifndef UTILS_H_
#define UTILS_H_

// Utility macros for vector operations

#define dot(v0, v1)                                                            \
  ({                                                                           \
    const vec2f_t _v0 = (v0), _v1 = (v1);                                      \
    (_v0.x * _v1.x) + (_v0.y * _v1.y);                                         \
  })
#define length(v)                                                              \
  ({                                                                           \
    const vec2f_t _v = (v);                                                    \
    sqrtf(dot(_v, _v));                                                        \
  })
#define normalize(u)                                                           \
  ({                                                                           \
    const vec2f_t _u = (u);                                                    \
    const double l = length(_u);                                               \
    (vec2f_t){_u.x / l, _u.y / l};                                             \
  })
#define min(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) _a = (a), _b = (b);                                          \
    _a < _b ? _a : _b;                                                         \
  })
#define max(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) _a = (a), _b = (b);                                          \
    _a > _b ? _a : _b;                                                         \
  })
#define sign(a)                                                                \
  ({                                                                           \
    __typeof__(a) _a = (a);                                                    \
    (__typeof__(a))(_a < 0 ? -1 : (_a > 0 ? 1 : 0));                           \
  })

// Vector types
typedef struct {
  float x, y;
} vec2f_t;
typedef struct {
  int x, y;
} vec2i_t;

#endif // UTILS_H_
