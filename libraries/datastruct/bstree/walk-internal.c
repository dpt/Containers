/* --------------------------------------------------------------------------
 *    Name: walk-internal.c
 * Purpose: Associative array implemented as a binary search tree
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/errors.h"

#include "datastruct/bstree.h"

#include "impl.h"

static error bstree__node_walk_internal_post(bstree__node_t                 *n,
                                             int                             level,
                                             bstree__walk_internal_callback *cb,
                                             void                           *opaque)
{
  error err;

  if (n == NULL)
    return error_OK;

  err = bstree__node_walk_internal_post(n->child[0], level + 1, cb, opaque);
  if (!err)
    err = bstree__node_walk_internal_post(n->child[1], level + 1, cb, opaque);
  if (!err)
    err = cb(n, level, opaque);

  return err;
}

error bstree__walk_internal_post(bstree_t                       *t,
                                 bstree__walk_internal_callback *cb,
                                 void                           *opaque)
{
  if (t == NULL)
    return error_OK;

  return bstree__node_walk_internal_post(t->root, 0, cb, opaque);
}

static error bstree__node_walk_internal(bstree__node_t                 *n,
                                        int                             level,
                                        bstree__walk_internal_callback *cb,
                                        void                           *opaque)
{
  error err;

  if (n == NULL)
    return error_OK;

  err = bstree__node_walk_internal(n->child[0], level + 1, cb, opaque);
  if (!err)
    err = cb(n, level, opaque);
  if (!err)
    err = bstree__node_walk_internal(n->child[1], level + 1, cb, opaque);

  return err;
}

/* in-order */
error bstree__walk_internal(bstree_t                       *t,
                            bstree__walk_internal_callback *cb,
                            void                           *opaque)
{
  if (t == NULL)
    return error_OK;

  return bstree__node_walk_internal(t->root, 0, cb, opaque);
}

