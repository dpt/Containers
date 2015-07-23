/* --------------------------------------------------------------------------
 *    Name: destroy.c
 * Purpose: Associative array implemented as a linked list
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "base/result.h"

#include "datastruct/linkedlist.h"

#include "impl.h"

static result_t linkedlist__destroy_node(linkedlist__node_t *n,
                                         void               *opaque)
{
  linkedlist__node_destroy(opaque, n);

  return result_OK;
}

void linkedlist_destroy(linkedlist_t *t)
{
  (void) linkedlist__walk_internal(t, linkedlist__destroy_node, t);

  free(t);
}

