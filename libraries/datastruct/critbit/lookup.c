/* --------------------------------------------------------------------------
 *    Name: lookup.c
 * Purpose: Associative array implemented as a critbit tree
 * ----------------------------------------------------------------------- */

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "datastruct/critbit.h"

#include "impl.h"

const critbit__extnode_t *critbit__lookup(const critbit__node_t *n,
                                          const void            *key,
                                          size_t                 keylen)
{
  const unsigned char *ukey    = key;
  const unsigned char *ukeyend = ukey + keylen;
  int                  dir;

  for (; IS_INTERNAL(n); n = n->child[dir])
    dir = GET_DIR(ukey, ukeyend, n->byte, n->otherbits);

  return FROM_STORE(n);
}

const void *critbit_lookup(const critbit_t *t,
                           const void      *key,
                           size_t           keylen)
{
  const critbit__extnode_t *n;

  assert(key != NULL);
  assert(keylen > 0);

  /* test for empty tree */
  if (t->root == NULL)
    return NULL;

  n = critbit__lookup(t->root, key, keylen);

  assert(n != NULL);
  if (n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0)
    return n->item.value; /* found */
  else
    return t->default_value; /* not found */
}

