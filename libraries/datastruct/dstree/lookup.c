/* --------------------------------------------------------------------------
 *    Name: lookup.c
 * Purpose: Associative array implemented as a digital search tree
 * ----------------------------------------------------------------------- */

#include <string.h>

#include "datastruct/dstree.h"

#include "impl.h"

const void *dstree_lookup(dstree_t *t, const void *key, size_t keylen)
{
  const unsigned char  *ukey    = key;
  const unsigned char  *ukeyend = ukey + keylen;
  int                   depth;
  const dstree__node_t *n;
  int                   dir;
  unsigned char         c = 0;

  depth = 0;

  for (n = t->root; n; n = n->child[dir])
  {
    if (n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0)
      return n->item.value; /* found */

    GET_NEXT_DIR(dir, ukey, ukeyend);
  }

  return t->default_value; /* not found */
}

