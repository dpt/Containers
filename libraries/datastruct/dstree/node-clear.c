/* --------------------------------------------------------------------------
 *    Name: node-clear.c
 * Purpose: Associative array implemented as a digital search tree
 * ----------------------------------------------------------------------- */

#include "datastruct/dstree.h"

#include "impl.h"

void dstree__node_clear(dstree_t *t, dstree__node_t *n)
{
  if (t->destroy_key)
    t->destroy_key((void *) n->item.key); /* must cast away const */
  if (t->destroy_value)
    t->destroy_value((void *) n->item.value);
}

