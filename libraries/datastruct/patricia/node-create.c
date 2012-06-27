/* --------------------------------------------------------------------------
 *    Name: node-create.c
 * Purpose: Associative array implemented as a PATRICIA tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <stdlib.h>

#include "base/memento/memento.h"

#include "datastruct/patricia.h"

#include "impl.h"

patricia__node_t *patricia__node_create(patricia_t *t,
                                        const void *key,
                                        size_t      keylen,
                                        const void *value)
{
  patricia__node_t *n;

  n = malloc(sizeof(*n));
  if (n == NULL)
    return NULL;

  n->child[0]    = NULL;
  n->child[1]    = NULL;
  n->item.key    = key;
  n->item.keylen = keylen;
  n->item.value  = value;
  n->bit         = 0xdeadbeef; /* expected to be overwritten subsequently */

  t->count++;

  return n;
}

