/* --------------------------------------------------------------------------
 *    Name: lookup.c
 * Purpose: Associative array implemented as a linked list
 * ----------------------------------------------------------------------- */

#include <string.h>

#include "datastruct/linkedlist.h"

#include "impl.h"

const void *linkedlist_lookup(linkedlist_t *t,
                              const void   *key,
                              size_t        keylen)
{
  linkedlist__node_t *n;

  for (n = t->anchor; n; n = n->next)
    if (n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0)
      break;

  return n ? n->item.value : t->default_value;
}

