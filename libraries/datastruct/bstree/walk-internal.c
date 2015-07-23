/* --------------------------------------------------------------------------
 *    Name: walk-internal.c
 * Purpose: Associative array implemented as a binary search tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <stdlib.h>

#include "base/result.h"

#include "datastruct/bstree.h"

#include "impl.h"

/* post-order (which allows for deletions) */
static result_t bstree__node_walk_internal_post(bstree__node_t                 *n,
                                                int                             level,
                                                bstree__walk_internal_callback *cb,
                                                void                           *opaque)
{
  result_t err;

  if (n == NULL)
    return result_OK;

  err = bstree__node_walk_internal_post(n->child[0], level + 1, cb, opaque);
  if (!err)
    err = bstree__node_walk_internal_post(n->child[1], level + 1, cb, opaque);
  if (!err)
    err = cb(n, level, opaque);

  return err;
}

result_t bstree__walk_internal_post(bstree_t                       *t,
                                    bstree__walk_internal_callback *cb,
                                    void                           *opaque)
{
  if (t == NULL)
    return result_OK;

  return bstree__node_walk_internal_post(t->root, 0, cb, opaque);
}

static result_t bstree__node_walk_internal(bstree__node_t                 *n,
                                           int                             level,
                                           bstree__walk_internal_callback *cb,
                                           void                           *opaque)
{
  result_t err;

  if (n == NULL)
    return result_OK;

  err = bstree__node_walk_internal(n->child[0], level + 1, cb, opaque);
  if (!err)
    err = cb(n, level, opaque);
  if (!err)
    err = bstree__node_walk_internal(n->child[1], level + 1, cb, opaque);

  return err;
}

/* in-order */
result_t bstree__walk_internal(bstree_t                       *t,
                               bstree__walk_internal_callback *cb,
                               void                           *opaque)
{
  if (t == NULL)
    return result_OK;

  return bstree__node_walk_internal(t->root, 0, cb, opaque);
}

