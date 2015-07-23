/* --------------------------------------------------------------------------
 *    Name: walk-internal.c
 * Purpose: Associative array implemented as a trie
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <stdlib.h>

#include "base/result.h"

#include "datastruct/trie.h"

#include "impl.h"

static result_t trie__walk_internal_in_order(trie__node_t                 *n,
                                             trie_walk_flags               flags,
                                             int                           level,
                                             trie__walk_internal_callback *cb,
                                             void                         *opaque)
{
  result_t err;

  if (n == NULL)
    return result_OK;

  err = trie__walk_internal_in_order(n->child[0], flags, level + 1, cb, opaque);
  if (!err)
  {
    int leaf;

    leaf = IS_LEAF(n);

    if (((flags & trie_WALK_LEAVES)   != 0 && leaf) ||
        ((flags & trie_WALK_BRANCHES) != 0 && !leaf))
      err = cb(n, level, opaque);
  }
  if (!err)
    err = trie__walk_internal_in_order(n->child[1], flags, level + 1, cb, opaque);

  return err;
}

static result_t trie__walk_internal_pre_order(trie__node_t                 *n,
                                              trie_walk_flags               flags,
                                              int                           level,
                                              trie__walk_internal_callback *cb,
                                              void                         *opaque)
{
  result_t err;

  if (n == NULL)
    return result_OK;

  {
    int leaf;

    leaf = IS_LEAF(n);

    if (((flags & trie_WALK_LEAVES)   != 0 && leaf) ||
        ((flags & trie_WALK_BRANCHES) != 0 && !leaf))
      err = cb(n, level, opaque);
    else
      err = result_OK;
  }
  if (!err)
    err = trie__walk_internal_pre_order(n->child[0], flags, level + 1, cb, opaque);
  if (!err)
    err = trie__walk_internal_pre_order(n->child[1], flags, level + 1, cb, opaque);

  return err;
}

static result_t trie__walk_internal_post_order(trie__node_t                 *n,
                                               trie_walk_flags               flags,
                                               int                           level,
                                               trie__walk_internal_callback *cb,
                                               void                         *opaque)
{
  result_t err;

  if (n == NULL)
    return result_OK;

  err = trie__walk_internal_post_order(n->child[0], flags, level + 1, cb, opaque);
  if (!err)
    err = trie__walk_internal_post_order(n->child[1], flags, level + 1, cb, opaque);
  if (!err)
  {
    int leaf;

    leaf = IS_LEAF(n);

    if (((flags & trie_WALK_LEAVES)   != 0 && leaf) ||
        ((flags & trie_WALK_BRANCHES) != 0 && !leaf))
      err = cb(n, level, opaque);
  }

  return err;
}

result_t trie__walk_internal(trie_t                       *t,
                             trie_walk_flags               flags,
                             trie__walk_internal_callback *cb,
                             void                         *opaque)
{
  result_t (*walker)(trie__node_t                 *n,
                     trie_walk_flags               flags,
                     int                           level,
                     trie__walk_internal_callback *cb,
                     void                         *opaque);

  if (t == NULL)
    return result_OK;

  switch (flags & trie_WALK_ORDER_MASK)
  {
  default:
  case trie_WALK_IN_ORDER:
    walker = trie__walk_internal_in_order;
    break;

  case trie_WALK_PRE_ORDER:
    walker = trie__walk_internal_pre_order;
    break;

  case trie_WALK_POST_ORDER:
    walker = trie__walk_internal_post_order;
    break;
  }

  return walker(t->root, flags, 0, cb, opaque);
}
