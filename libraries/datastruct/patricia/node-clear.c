/* --------------------------------------------------------------------------
 *    Name: node-clear.c
 * Purpose: Associative array implemented as a PATRICIA tree
 * ----------------------------------------------------------------------- */

#include "datastruct/critbit.h"

#include "impl.h"

void patricia__node_clear(patricia_t *t, patricia__node_t *n)
{
  if (t->destroy_key && n->item.key)
    t->destroy_key((void *) n->item.key); /* must cast away const */
  if (t->destroy_value && n->item.value)
    t->destroy_value((void *) n->item.value);
}

