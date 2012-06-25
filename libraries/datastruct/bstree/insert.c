/* --------------------------------------------------------------------------
 *    Name: insert.c
 * Purpose: Associative array implemented as a binary search tree
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/errors.h"
#include "base/types.h"

#include "datastruct/bstree.h"

#include "impl.h"

static INLINE bstree__node_t **bstree__insert_node(bstree__node_t **pn,
                                                   const void      *key,
                                                   bstree_compare  *compare)
{
  bstree__node_t *n;

  while ((n = *pn) != NULL)
  {
    int d;

    d = compare(key, n->item.key);
    if (d == 0)
      return NULL; /* found match */

    pn = &n->child[d < 0 ? 0 : 1];
  }

  return pn; /* found insertion point */
}

error bstree_insert(bstree_t   *t,
                    const void *key,
                    size_t      keylen,
                    const void *value)
{
  bstree__node_t **pn;

  pn = bstree__insert_node(&t->root, key, t->compare);
  if (pn == NULL)
    return error_EXISTS;

  *pn = bstree__node_create(t, key, keylen, value);
  if (*pn == NULL)
    return error_OOM;

  return error_OK;
}

