/* --------------------------------------------------------------------------
 *    Name: node-create.c
 * Purpose: Associative array implemented as a digital search tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <stdlib.h>

#include "base/memento/memento.h"

#include "datastruct/dstree.h"

#include "impl.h"

dstree__node_t *dstree__node_create(dstree_t   *t,
                                    const void *key,
                                    const void *value,
                                    size_t      keylen)
{
  dstree__node_t *n;

  n = malloc(sizeof(*n));
  if (n == NULL)
    return NULL;

  n->child[0]    = NULL;
  n->child[1]    = NULL;
  n->item.key    = key;
  n->item.keylen = keylen;
  n->item.value  = value;

  t->count++;

  return n;
}

