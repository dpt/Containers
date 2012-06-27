/* --------------------------------------------------------------------------
 *    Name: select.c
 * Purpose: Associative array implemented as a linked list
 * ----------------------------------------------------------------------- */

#include "datastruct/item.h"

#include "datastruct/linkedlist.h"

#include "impl.h"

const item_t *linkedlist_select(linkedlist_t *t, int k)
{
  linkedlist__node_t *n;

  for (n = t->anchor; n; n = n->next)
    if (k-- == 0)
      break;

  return n ? &n->item : NULL;
}

