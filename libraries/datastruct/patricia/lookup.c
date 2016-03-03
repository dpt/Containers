/* --------------------------------------------------------------------------
 *    Name: lookup.c
 * Purpose: Associative array implemented as a PATRICIA tree
 * ----------------------------------------------------------------------- */

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "base/types.h"
#include "base/utils.h"

#include "utils/utils.h"

#include "datastruct/patricia.h"

#include "impl.h"

const patricia__node_t *patricia__lookup(const patricia__node_t *n,
                                         const void             *key,
                                         size_t                  keylen)
{
  const unsigned char *ukey    = key;
  const unsigned char *ukeyend = ukey + keylen;
  int                  i;

  /* We follow nodes until we hit a lower bit value than previously
   * encountered. Sedgewick has values increasing as we progress through the
   * tree. Other texts describe it the other way around with the values
   * descending but that requires known key lengths. */

  do
  {
    i = n->bit;
    n = n->child[GET_DIR(ukey, ukeyend, i)];
    assert(n != NULL);
  }
  while (n->bit > i); /* we encounter ascending bit indices */

  return n;
}

const void *patricia_lookup(const patricia_t *t,
                            const void       *key,
                            size_t            keylen)
{
  const patricia__node_t *n;

  assert(key != NULL);
  assert(keylen > 0);

  /* test for empty tree */
  n = t->root;
  if (n == NULL)
    return NULL;

  /* keys consisting of all zero bits always live in the root node */
  if (unlikely(iszero(key, keylen)))
    return n->item.value; /* found */

  n = patricia__lookup(n, key, keylen);

  assert(n != NULL);
  if (n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0)
    return n->item.value; /* found */
  else
    return t->default_value; /* not found */
}

