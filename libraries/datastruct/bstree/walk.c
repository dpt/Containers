/* --------------------------------------------------------------------------
 *    Name: walk.c
 * Purpose: Associative array implemented as a binary search tree
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/types.h"

#include "datastruct/bstree.h"

#include "impl.h"

static result_t walk_in_order(const bstree__node_t *n,
                              bstree_walk_flags     flags,
                              int                   level,
                              bstree_walk_callback *cb,
                              void                 *opaque)
{
  result_t err;

  if (n == NULL)
    return result_OK;

  err = walk_in_order(n->child[0], flags, level + 1, cb, opaque);
  if (!err)
    err = cb(&n->item, level, opaque);
  if (!err)
    err = walk_in_order(n->child[1], flags, level + 1, cb, opaque);

  return err;
}

static result_t walk_pre_order(const bstree__node_t *n,
                               bstree_walk_flags     flags,
                               int                   level,
                               bstree_walk_callback *cb,
                               void                 *opaque)
{
  result_t err;

  if (n == NULL)
    return result_OK;

  err = cb(&n->item, level, opaque);
  if (!err)
    err = walk_in_order(n->child[0], flags, level + 1, cb, opaque);
  if (!err)
    err = walk_in_order(n->child[1], flags, level + 1, cb, opaque);

  return err;
}

static result_t walk_post_order(const bstree__node_t *n,
                                bstree_walk_flags     flags,
                                int                   level,
                                bstree_walk_callback *cb,
                                void                 *opaque)
{
  result_t err;

  if (n == NULL)
    return result_OK;

  err = walk_in_order(n->child[0], flags, level + 1, cb, opaque);
  if (!err)
    err = walk_in_order(n->child[1], flags, level + 1, cb, opaque);
  if (!err)
    err = cb(&n->item, level, opaque);

  return err;
}

result_t bstree_walk(const bstree_t       *t,
                     bstree_walk_flags     flags,
                     bstree_walk_callback *cb,
                     void                 *opaque)
{
  result_t (*walker)(const bstree__node_t *n,
                     bstree_walk_flags     flags,
                     int                   level,
                     bstree_walk_callback *cb,
                     void                 *opaque);

  if (t == NULL)
    return result_OK;

  // does not yet implement bstree_WALK_BRANCHES etc

  switch (flags & bstree_WALK_ORDER_MASK)
  {
  default:
  case bstree_WALK_IN_ORDER:
    walker = walk_in_order;
    break;

  case bstree_WALK_PRE_ORDER:
    walker = walk_pre_order;
    break;

  case bstree_WALK_POST_ORDER:
    walker = walk_post_order;
    break;
  }

  return walker(t->root, flags, 0, cb, opaque);
}

