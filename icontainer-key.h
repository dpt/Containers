/* icontainer-key.h -- interface of keys */

#ifndef ICONTAINER_KEY_H
#define ICONTAINER_KEY_H

#include "icontainer-kv.h"

/* Return the length of the specified key. */
typedef size_t (*icontainer_key_len)(const void *key);

/* Compare two keys (as for qsort). */
typedef int (*icontainer_key_compare)(const void *a, const void *b);

typedef struct icontainer_key
{
  icontainer_key_len     len;
  icontainer_key_compare compare;
  icontainer_kv_t        kv;
}
icontainer_key_t;

#endif /* ICONTAINER_KEY_H */
