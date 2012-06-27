/* --------------------------------------------------------------------------
 *    Name: remove.c
 * Purpose: Associative array implemented as an ordered array
 * ----------------------------------------------------------------------- */

#include "datastruct/orderedarray.h"

#include "impl.h"

void orderedarray_remove(orderedarray_t *t, const void *key)
{
  orderedarray__node_t *n;

  if (!orderedarray__lookup_internal(t, key, &n))
    return; /* not found */

  orderedarray__node_destroy(t, n);
}

