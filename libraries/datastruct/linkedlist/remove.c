/* --------------------------------------------------------------------------
 *    Name: remove.c
 * Purpose: Linked list
 * ----------------------------------------------------------------------- */

#include <string.h>

#include "datastruct/linkedlist.h"

#include "impl.h"

void linkedlist_remove(linkedlist_t *t, const void *key, size_t keylen)
{
  linkedlist__node_t **pn;
  linkedlist__node_t  *n;

  for (pn = &t->anchor; (n = *pn); pn = &(*pn)->next)
    if (n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0)
      break;

  if (n == NULL)
    return; /* not found */

  *pn = n->next;

  linkedlist__node_destroy(t, n);
}

