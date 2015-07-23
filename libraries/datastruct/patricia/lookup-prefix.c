/* --------------------------------------------------------------------------
 *    Name: lookup-prefix.c
 * Purpose: Associative array implemented as a PATRICIA tree
 * ----------------------------------------------------------------------- */

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "base/result.h"

#include "datastruct/patricia.h"

#include "impl.h"

/* This is similar to patricia__walk_in_order, but returns items as an item_t
 * pointer. */
static result_t patricia__lookup_prefix_walk(const patricia__node_t  *n,
                                             const void              *prefix,
                                             size_t                   prefixlen,
                                             patricia_found_callback *cb,
                                             void                    *opaque)
{
  result_t err;
  int      i;

  if (n == NULL)
    return result_OK;

  for (i = 0; i < 2; i++)
  {
    const patricia__node_t *c = n->child[i];

    if (c == NULL)
      continue;

    /* does this child point to a leaf? */
    if (c->bit <= n->bit)
    {
      // assert(c->item.keylen >= prefixlen);

      //if (c->item.keylen >= prefixlen &&
      //    memcmp(c->item.key, prefix, prefixlen) == 0)
      {
        err = cb(&c->item, opaque);
        if (err)
          return err;
      }
    }
    else /* branch */
    {
      err = patricia__lookup_prefix_walk(c, prefix, prefixlen, cb, opaque);
      if (err)
        return err;
    }
  }

  return result_OK;
}

result_t patricia_lookup_prefix(const patricia_t        *t,
                                const void              *prefix,
                                size_t                   prefixlen,
                                patricia_found_callback *cb,
                                void                    *opaque)
{
  const unsigned char *uprefix    = prefix;
  const unsigned char *uprefixend = uprefix + prefixlen;
  patricia__node_t    *n;
  patricia__node_t    *top;
  int                  i;

  n   = t->root;
  top = NULL;

  if (n == NULL)
    return result_OK; /* empty tree */

  /* The patricia trie tells us how to branch for a given bit but not what
   * the intermediate bits were. This means we can begin to search the trie
   * by prefix and end up in totally the wrong place. */

  /* search for the prefix as best as we can */
  do
  {
    i = n->bit;
    n = n->child[GET_DIR(uprefix, uprefixend, i)];
    assert(n != NULL);

    /* remember the root of the subtree which best matches the prefix */
    if (i < (int) prefixlen * 8) /* i can be negative */
      top = n;
  }
  while (n->bit > i); /* we encounter ascending bit indices */

  /* ensure the prefix exists */
  if (n->item.keylen < prefixlen ||
      memcmp(n->item.key, prefix, prefixlen) != 0)
    return result_NOT_FOUND;

  if (prefixlen * 8 - 1 == (size_t) i /* not n->bit */)
  {
    // single exact match?
    return result_OK;
  }

  assert(top != NULL);

  return patricia__lookup_prefix_walk(top, prefix, prefixlen, cb, opaque);
}

