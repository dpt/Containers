/* --------------------------------------------------------------------------
 *    Name: node-destroy.c
 * Purpose: Linked list
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "datastruct/linkedlist.h"

#include "impl.h"

void linkedlist__node_destroy(linkedlist_t       *t,
                              linkedlist__node_t *n)
{
  if (t->destroy_key)
    t->destroy_key((void *) n->item.key); /* must cast away const */
  if (t->destroy_value)
    t->destroy_value((void *) n->item.value);

  free(n);

  t->count--;
}

