/* --------------------------------------------------------------------------
 *    Name: walk-internal.c
 * Purpose: Associative array implemented as a digital search tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <stdlib.h>

#include "base/errors.h"

#include "datastruct/dstree.h"

#include "impl.h"

/* post-order (which allows for deletions) */
static error dstree__node_walk_internal_post(dstree__node_t                 *n,
                                             int                             level,
                                             dstree__walk_internal_callback *cb,
                                             void                           *opaque)
{
  error err;

  if (n == NULL)
    return error_OK;

  err = dstree__node_walk_internal_post(n->child[0], level + 1, cb, opaque);
  if (!err)
    err = dstree__node_walk_internal_post(n->child[1], level + 1, cb, opaque);
  if (!err)
    err = cb(n, level, opaque);

  return err;
}

error dstree__walk_internal_post(dstree_t                       *t,
                                 dstree__walk_internal_callback *cb,
                                 void                           *opaque)
{
  if (t == NULL)
    return error_OK;

  return dstree__node_walk_internal_post(t->root, 0, cb, opaque);
}

error dstree__walk_internal_post_node(dstree__node_t                 *root,
                                      int                             level,
                                      dstree__walk_internal_callback *cb,
                                      void                           *opaque)
{
  if (root == NULL)
    return error_OK;

  return dstree__node_walk_internal_post(root, level, cb, opaque);
}

static error dstree__node_walk_internal(dstree__node_t                 *n,
                                        int                             level,
                                        dstree__walk_internal_callback *cb,
                                        void                           *opaque)
{
  error err;

  if (n == NULL)
    return error_OK;

  err = dstree__node_walk_internal(n->child[0], level + 1, cb, opaque);
  if (!err)
    err = cb(n, level, opaque);
  if (!err)
    err = dstree__node_walk_internal(n->child[1], level + 1, cb, opaque);

  return err;
}

/* in-order */
error dstree__walk_internal(dstree_t                       *t,
                            dstree__walk_internal_callback *cb,
                            void                           *opaque)
{
  if (t == NULL)
    return error_OK;

  return dstree__node_walk_internal(t->root, 0, cb, opaque);
}

