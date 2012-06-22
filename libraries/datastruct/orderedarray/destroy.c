/* --------------------------------------------------------------------------
 *    Name: destroy.c
 * Purpose: Ordered array
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "base/errors.h"

#include "datastruct/orderedarray.h"

#include "impl.h"

static error orderedarray__destroy_node(orderedarray__node_t *n,
                                        void                 *opaque)
{
  orderedarray__node_destroy(opaque, n);

  return error_OK;
}

void orderedarray_destroy(orderedarray_t *t)
{
  (void) orderedarray__walk_internal(t, orderedarray__destroy_node, t);

  free(t->array);

  free(t);
}

