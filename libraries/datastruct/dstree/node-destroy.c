/* --------------------------------------------------------------------------
 *    Name: node-destroy.c
 * Purpose: Associative array implemented as a digital search tree
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "datastruct/critbit.h"

#include "impl.h"

void dstree__node_destroy(dstree_t *t, dstree__node_t *n)
{
  if (t->destroy_key)
    t->destroy_key((void *) n->item.key); /* must cast away const */
  if (t->destroy_value)
    t->destroy_value((void *) n->item.value);

  free(n);

  t->count--;
}

