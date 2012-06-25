/* --------------------------------------------------------------------------
 *    Name: node-destroy.c
 * Purpose: Associative array implemented as a binary search tree
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "datastruct/bstree.h"

#include "impl.h"

void bstree__node_destroy_item(bstree_t *t, bstree__node_t *n)
{
  if (t->destroy_key)
    t->destroy_key((void *) n->item.key); /* must cast away const */
  if (t->destroy_value)
    t->destroy_value((void *) n->item.value);
}

void bstree__node_destroy(bstree_t *t, bstree__node_t *n)
{
  bstree__node_destroy_item(t, n);

  free(n);

  t->count--;
}

