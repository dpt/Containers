/* --------------------------------------------------------------------------
 *    Name: destroy.c
 * Purpose: Associative array implemented as an ordered array
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "base/result.h"

#include "datastruct/orderedarray.h"

#include "impl.h"

static result_t orderedarray__destroy_node(orderedarray__node_t *n,
                                           void                 *opaque)
{
  orderedarray__node_destroy(opaque, n);

  return result_OK;
}

void orderedarray_destroy(orderedarray_t *t)
{
  (void) orderedarray__walk_internal(t, orderedarray__destroy_node, t);

  free(t->array);

  free(t);
}

