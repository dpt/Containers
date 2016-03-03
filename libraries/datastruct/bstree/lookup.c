/* --------------------------------------------------------------------------
 *    Name: lookup.c
 * Purpose: Associative array implemented as a binary search tree
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/result.h"
#include "base/types.h"
#include "base/utils.h"

#include "datastruct/bstree.h"

#include "impl.h"

static INLINE const void *bstree__lookup_node(const bstree__node_t *n,
                                              const void           *key,
                                              const void           *default_value,
                                              bstree_compare       *compare)
{
  while (n)
  {
    int d;

    d = compare(key, n->item.key);
    if (d == 0)
      return n->item.value; /* found */

    n = n->child[d < 0 ? 0 : 1];
  }

  return default_value; /* not found */
}

const void *bstree_lookup(bstree_t *t, const void *key)
{
  return bstree__lookup_node(t->root, key, t->default_value, t->compare);
}

