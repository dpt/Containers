/* --------------------------------------------------------------------------
 *    Name: lookup.c
 * Purpose: Associative array implemented as a trie
 * ----------------------------------------------------------------------- */

#include <string.h>

#include "datastruct/trie.h"

#include "impl.h"

const void *trie_lookup(trie_t *t, const void *key, size_t keylen)
{
  const unsigned char *ukey    = key;
  const unsigned char *ukeyend = ukey + keylen;
  int                  depth;
  const trie__node_t  *n;
  int                  dir;
  unsigned char        c = 0;

  depth = 0;

  for (n = t->root; n; n = n->child[dir])
  {
    if (IS_LEAF(n))
      break;

    GET_NEXT_DIR(dir, ukey, ukeyend);
  }

  if (n && n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0)
    return n->item.value; /* found */
  else
    return t->default_value; /* not found */
}
