/* --------------------------------------------------------------------------
 *    Name: node-create.c
 * Purpose: Associative array implemented as a binary search tree
 * ----------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>

#include "base/memento/memento.h"

#include "datastruct/bstree.h"

#include "impl.h"

bstree__node_t *bstree__node_create(bstree_t   *t,
                                    const void *key,
                                    size_t      keylen,
                                    const void *value)
{
  bstree__node_t *n;

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


