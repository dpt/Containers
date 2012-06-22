/* --------------------------------------------------------------------------
 *    Name: node-destroy.c
 * Purpose: Ordered array
 * ----------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>

#include "base/memento/memento.h"

#include "datastruct/orderedarray.h"

#include "impl.h"

void orderedarray__node_destroy(orderedarray_t       *t,
                                orderedarray__node_t *n)
{
  size_t m;

  if (t->destroy_key)
    t->destroy_key((void *) n->item.key); /* must cast away const */
  if (t->destroy_value)
    t->destroy_value((void *) n->item.value);

  m = t->nelems - (n - t->array + 1);

  if (m)
    memmove(n, n + 1, m * sizeof(*t->array));

  t->nelems--;
}

