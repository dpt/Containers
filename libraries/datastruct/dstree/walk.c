/* --------------------------------------------------------------------------
 *    Name: walk.c
 * Purpose: Associative array implemented as a digital search tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>

#include "base/errors.h"

#include "datastruct/dstree.h"

#include "impl.h"

static error dstree__node_walk(const dstree__node_t *n,
                               int                   level,
                               dstree_walk_callback *cb,
                               void                 *opaque)
{
  error err;

  if (n == NULL)
    return error_OK;

  err = dstree__node_walk(n->child[0], level + 1, cb, opaque);
  if (!err)
    err = cb(&n->item, level, opaque);
  if (!err)
    err = dstree__node_walk(n->child[1], level + 1, cb, opaque);

  return err;
}

/* in-order */
error dstree_walk(const dstree_t       *t,
                  dstree_walk_callback *cb,
                  void                 *opaque)
{
  if (t == NULL)
    return error_OK;

  return dstree__node_walk(t->root, 0, cb, opaque);
}

