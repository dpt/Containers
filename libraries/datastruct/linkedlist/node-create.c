/* --------------------------------------------------------------------------
 *    Name: node-create.c
 * Purpose: Associative array implemented as a linked list
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "datastruct/linkedlist.h"

#include "impl.h"

linkedlist__node_t *linkedlist__node_create(linkedlist_t *t,
                                            const void   *key,
                                            size_t        keylen,
                                            const void   *value)
{
  linkedlist__node_t *n;

  n = malloc(sizeof(*n));
  if (n == NULL)
    return NULL;

  n->next        = NULL;
  n->item.key    = key;
  n->item.keylen = keylen;
  n->item.value  = value;

  t->count++;

  return n;
}

