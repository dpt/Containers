/* utils.h -- utilities */

#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

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
