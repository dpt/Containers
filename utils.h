/* utils.h -- utilities */

#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

#define NELEMS(x) ((int) (sizeof(x) / sizeof(x[0])))

#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define NOT_USED(x) ((x) = (x))

#ifdef _WIN32
#define INLINE __inline
#else
#define INLINE __inline__
#endif

#ifdef __GNUC__
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x)   (x)
#define unlikely(x) (x)
#endif

/* Return the bit index at which key1 and key2 differ.
 * Bits within bytes are numbered MSB first, i.e. bit index zero is the bit
 * worth 128 in the first byte, bit index 27 is the bit worth 4 in the third
 * byte.
 * This is unusual, but produces better results when graphing collections of
 * strings.
 */
int keydiffbit(const unsigned char *key1, size_t key1len,
               const unsigned char *key2, size_t key2len);

#endif /* UTILS_H */
