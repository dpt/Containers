/* utils.h -- various utilities */

#ifndef BASE_UTILS_H
#define BASE_UTILS_H

/**
 * Returns the number of elements in the specified array.
 */
#ifndef NELEMS
#define NELEMS(a) ((int) (sizeof(a) / sizeof((a)[0])))
#endif

/**
 * Return the minimum of (a,b).
 */
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

/**
 * Return the maximum of (a,b).
 */
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

/**
 * Return 'a' clamped to the range [b..c].
 */
#define CLAMP(a,b,c) MIN(MAX(a,b),c)

/**
 * Suppress warnings about unused variables.
 */
#define NOT_USED(x) ((x) = (x))

/**
 * Inlining.
 */
#ifdef _WIN32
#define INLINE __inline
#else
#define INLINE __inline__
#endif

/**
 * Hints to compiler of probable execution path.
 */
#ifdef __GNUC__
#define likely(expr)   __builtin_expect(!!(expr), 1)
#define unlikely(expr) __builtin_expect(!!(expr), 0)
#else
#define likely(expr)   (expr)
#define unlikely(expr) (expr)
#endif

#endif /* BASE_UTILS_H */
