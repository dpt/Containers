/* --------------------------------------------------------------------------
 *    Name: lookup-prefix.c
 * Purpose: Associative array implemented as a digital search tree
 * ----------------------------------------------------------------------- */

#include <string.h>

#include "base/errors.h"

#include "datastruct/dstree.h"

#include "impl.h"

typedef struct dstree_lookup_prefix_args
{
  const unsigned char   *uprefix;
  size_t                 prefixlen;
  dstree_found_callback *cb;
  void                  *opaque;
}
dstree_lookup_prefix_args_t;

static error dstree__lookup_prefix_node(dstree__node_t *n,
                                        int             level,
                                        void           *opaque)
{
  dstree_lookup_prefix_args_t *args = opaque;
  size_t                       prefixlen;

  NOT_USED(level);

  prefixlen = args->prefixlen;

  if (n->item.keylen >= prefixlen &&
      memcmp(n->item.key, args->uprefix, prefixlen) == 0)
  {
    return args->cb(&n->item, args->opaque);
  }
  else
  {
    return error_OK;
  }
}

error dstree_lookup_prefix(const dstree_t        *t,
                           const void            *prefix,
                           size_t                 prefixlen,
                           dstree_found_callback *cb,
                           void                  *opaque)
{
  error                       err;
  const unsigned char        *uprefix    = prefix;
  const unsigned char        *uprefixend = uprefix + prefixlen;
  int                         depth;
  const dstree__node_t       *n;
  unsigned char               c = 0;
  int                         dir;
  dstree_lookup_prefix_args_t args;
  int                         i;

  /* For a prefix P, keys which share that prefix are stored at a depth
   * proportional to the length of P. However, the nodes leading up to that
   * node may also contain valid prefixed nodes so we must approach this in
   * two stages:
   *
   * 1) Walk the tree until we hit a depth matching the length of the prefix,
   *    testing nodes individually to see if they match. Issue callbacks if
   *    they do.
   *
   * 2) Once the depths is met (if it exists) we can then enumerate the
   *    entire subtree, issuing callbacks for all nodes in the subtree.
   */

  err = error_NOT_FOUND; /* assume not found until we call the callback */

  depth = 0;

  for (n = t->root; n; n = n->child[dir])
  {
    if (n->item.keylen >= prefixlen &&
        memcmp(n->item.key, prefix, prefixlen) == 0)
    {
      err = cb(&n->item, opaque); /* match */
      if (err)
        return err;
    }

    GET_NEXT_DIR(dir, uprefix, uprefixend);

    if ((size_t) depth == prefixlen * 8)
      break; /* deep enough that all children must match the prefix */
  }

  if (!n)
    return err;

  args.uprefix   = prefix;
  args.prefixlen = prefixlen;
  args.cb        = cb;
  args.opaque    = opaque;

  /* We've processed the node sitting at the position where the prefix is.
   * Now process both of its sub-trees. */

  for (i = 0; i < 2; i++)
  {
    err = dstree__walk_internal_post_node((dstree__node_t *) n->child[i],
                                          depth,
                                          dstree__lookup_prefix_node,
                                          &args);
    if (err)
      return err;
  }

  return error_OK;
}

