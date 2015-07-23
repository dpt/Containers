/* --------------------------------------------------------------------------
 *    Name: lookup-prefix.c
 * Purpose: Associative array implemented as a binary search tree
 * ----------------------------------------------------------------------- */

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "base/result.h"

#include "datastruct/bstree.h"

#include "impl.h"

/* This is similar to critbit__walk_in_order, but returns items as an item_t
 * pointer. */
static result_t critbit__lookup_prefix_walk(const critbit__node_t  *n,
                                            critbit_found_callback *cb,
                                            void                   *opaque)
{
  result_t            err;
  critbit__extnode_t *e;

  if (IS_EXTERNAL(n))
  {
    e = FROM_STORE(n);

    err = cb(&e->item, opaque);
    if (err)
      return err;
  }
  else
  {
    err = critbit__lookup_prefix_walk(n->child[0], cb, opaque);
    if (err)
      return err;

    err = critbit__lookup_prefix_walk(n->child[1], cb, opaque);
    if (err)
      return err;
  }

  return result_OK;
}

result_t critbit_lookup_prefix(const critbit_t        *t,
                               const void             *prefix,
                               size_t                  prefixlen,
                               critbit_found_callback *cb,
                               void                   *opaque)
{
  const unsigned char   *uprefix    = prefix;
  const unsigned char   *uprefixend = uprefix + prefixlen;
  critbit__node_t       *n;
  int                    dir;
  const critbit__node_t *top;
  critbit__extnode_t    *e;

  n   = t->root;
  top = n;

  if (n == NULL)
    return result_OK; /* empty tree */

  while (IS_INTERNAL(n))
  {
    critbit__node_t *m;

    dir = GET_DIR(uprefix, uprefixend, n->byte, n->otherbits);

    m = n->child[dir];

    if ((size_t) n->byte < prefixlen)
      top = m;

    n = m;
  }

  e = FROM_STORE(n);

  /* ensure the prefix exists */
  if (e->item.keylen < prefixlen ||
      memcmp(e->item.key, prefix, prefixlen) != 0)
    return result_NOT_FOUND;

  assert(top != NULL);

  return critbit__lookup_prefix_walk(top, cb, opaque);
}

