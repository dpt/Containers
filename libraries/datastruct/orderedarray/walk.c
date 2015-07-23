/* --------------------------------------------------------------------------
 *    Name: walk.c
 * Purpose: Associative array implemented as a linked list
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/result.h"

#include "datastruct/orderedarray.h"

#include "impl.h"

result_t orderedarray_walk(const orderedarray_t       *t,
                           orderedarray_walk_callback *cb,
                           void                       *opaque)
{
  result_t              err;
  orderedarray__node_t *n;

  if (t == NULL)
    return result_OK;

  /* don't pre-calculate the end position as it needs to be evaluated on
   * every step: the callback is permitted to delete the current element */

  for (n = t->array; n < t->array + t->nelems; n++)
  {
    err = cb(&n->item, opaque);
    if (err)
      return err;
  }

  return result_OK;
}

