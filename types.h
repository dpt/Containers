#ifndef TYPES_H
#define TYPES_H

#include "errors.h"

#define NELEMS(x) ((int) (sizeof(x) / sizeof(x[0])))

#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define NOT_USED(x) ((x) = (x))

#ifdef __GNUC__
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x)   (x)
#define unlikely(x) (x)
#endif

typedef unsigned char uint8_t; // move elsewhere
typedef unsigned int uint32_t;

#endif /* TYPES_H */
