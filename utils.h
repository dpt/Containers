#ifndef UTILS_H
#define UTILS_H

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

#endif /* UTILS_H */
