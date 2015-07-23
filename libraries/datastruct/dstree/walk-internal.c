/* --------------------------------------------------------------------------
 *    Name: walk-internal.c
 * Purpose: Associative array implemented as a digital search tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <stdlib.h>

#include "base/result.h"

#include "datastruct/dstree.h"

#include "impl.h"

/* post-order (which allows for deletions) */
static result_t dstree__node_walk_internal_post(dstree__node_t                 *n,
                                                int                             level,
                                                dstree__walk_internal_callback *cb,
                                                void                           *opaque)
{
  result_t err;

  if (n == NULL)
    return result_OK;

  err = dstree__node_walk_internal_post(n->child[0], level + 1, cb, opaque);
  if (!err)
    err = dstree__node_walk_internal_post(n->child[1], level + 1, cb, opaque);
  if (!err)
    err = cb(n, level, opaque);

  return err;
}

result_t dstree__walk_internal_post(dstree_t                       *t,
                                    dstree__walk_internal_callback *cb,
                                    void                           *opaque)
{
  if (t == NULL)
    return result_OK;

  return dstree__node_walk_internal_post(t->root, 0, cb, opaque);
}

result_t dstree__walk_internal_post_node(dstree__node_t                 *root,
                                         int                             level,
                                         dstree__walk_internal_callback *cb,
                                         void                           *opaque)
{
  if (root == NULL)
    return result_OK;

  return dstree__node_walk_internal_post(root, level, cb, opaque);
}

static result_t dstree__node_walk_internal(dstree__node_t                 *n,
                                           int                             level,
                                           dstree__walk_internal_callback *cb,
                                           void                           *opaque)
{
  result_t err;

  if (n == NULL)
    return result_OK;

  err = dstree__node_walk_internal(n->child[0], level + 1, cb, opaque);
  if (!err)
    err = cb(n, level, opaque);
  if (!err)
    err = dstree__node_walk_internal(n->child[1], level + 1, cb, opaque);

  return err;
}

/* in-order */
result_t dstree__walk_internal(dstree_t                       *t,
                               dstree__walk_internal_callback *cb,
                               void                           *opaque)
{
  if (t == NULL)
    return result_OK;

  return dstree__node_walk_internal(t->root, 0, cb, opaque);
}

