/* --------------------------------------------------------------------------
 *    Name: lookup-prefix.c
 * Purpose: Associative array implemented as a trie
 * ----------------------------------------------------------------------- */

#include <string.h>

#include "base/errors.h"

#include "datastruct/trie.h"

#include "impl.h"

/* This is similar to trie__walk_in_order, but returns items as an item_t
 * pointer. */
static error trie__lookup_prefix_walk(const trie__node_t  *n,
                                      trie_found_callback *cb,
                                      void                *opaque)
{
  error err;

  if (n == NULL)
    return error_OK;

  if (IS_LEAF(n))
  {
    return cb(&n->item, opaque);
  }
  else
  {
    err = trie__lookup_prefix_walk(n->child[0], cb, opaque);
    if (!err)
      err = trie__lookup_prefix_walk(n->child[1], cb, opaque);
    return err;
  }
}

error trie_lookup_prefix(const trie_t        *t,
                         const void          *prefix,
                         size_t               prefixlen, // bytes
                         trie_found_callback *cb,
                         void                *opaque)
{
  const unsigned char *uprefix    = prefix;
  const unsigned char *uprefixend = uprefix + prefixlen;
  int                  depth;
  trie__node_t        *n;
  unsigned char        c = 0;
  int                  dir;

  if (t->root == NULL)
    return error_OK; /* empty tree */

  depth = 0;

  for (n = t->root; n; n = n->child[dir])
  {
    if (IS_LEAF(n) || (size_t) depth == prefixlen * 8)
      break;

    GET_NEXT_DIR(dir, uprefix, uprefixend);
  }

  /* If we tried to walk in a direction not present in the trie then the
   * prefix can't exist in it. */
  if (n == NULL)
    return error_NOT_FOUND;

  if (IS_LEAF(n))
  {
    /* We've found a leaf which may or may not match. If it matches then we
     * can call the callback for it. */
    if (n->item.keylen >= prefixlen &&
        memcmp(prefix, n->item.key, prefixlen) == 0)
      return cb(&n->item, opaque);
    else
      return error_NOT_FOUND;
  }
  else
  {
    return trie__lookup_prefix_walk(n, cb, opaque);
  }
}
