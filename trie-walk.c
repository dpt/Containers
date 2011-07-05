/* trie-walk.c -- associative array implemented as trie */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memento.h"

#include "types.h"
#include "string.h"
#include "string-kv.h"

#include "trie.h"

#include "trie-impl.h"

/* ----------------------------------------------------------------------- */

static error trie_walk_in_order(const trie__node_t *n,
                                trie_walk_flags     flags,
                                int                 level,
                                trie_walk_callback *cb,
                                void               *opaque)
{
  error err;

  if (n == NULL)
    return error_OK;

  err = trie_walk_in_order(n->child[0], flags, level + 1, cb, opaque);
  if (!err)
  {
    int leaf;

    leaf = IS_LEAF(n);

    if (((flags & trie_WALK_LEAVES)   != 0 && leaf) ||
        ((flags & trie_WALK_BRANCHES) != 0 && !leaf))
      err = cb(n->item.key, n->item.value, level, opaque);
  }
  if (!err)
    err = trie_walk_in_order(n->child[1], flags, level + 1, cb, opaque);

  return err;
}

static error trie_walk_pre_order(const trie__node_t *n,
                                 trie_walk_flags     flags,
                                 int                 level,
                                 trie_walk_callback *cb,
                                 void               *opaque)
{
  error err;

  if (n == NULL)
    return error_OK;

  {
    int leaf;

    leaf = IS_LEAF(n);

    if (((flags & trie_WALK_LEAVES)   != 0 && leaf) ||
        ((flags & trie_WALK_BRANCHES) != 0 && !leaf))
      err = cb(n->item.key, n->item.value, level, opaque);
    else
      err = error_OK;
  }
  if (!err)
    err = trie_walk_in_order(n->child[0], flags, level + 1, cb, opaque);
  if (!err)
    err = trie_walk_in_order(n->child[1], flags, level + 1, cb, opaque);

  return err;
}

static error trie_walk_post_order(const trie__node_t *n,
                                  trie_walk_flags     flags,
                                  int                 level,
                                  trie_walk_callback *cb,
                                  void               *opaque)
{
  error err;

  if (n == NULL)
    return error_OK;

  err = trie_walk_in_order(n->child[0], flags, level + 1, cb, opaque);
  if (!err)
    err = trie_walk_in_order(n->child[1], flags, level + 1, cb, opaque);
  if (!err)
  {
    int leaf;

    leaf = IS_LEAF(n);

    if (((flags & trie_WALK_LEAVES)   != 0 && leaf) ||
        ((flags & trie_WALK_BRANCHES) != 0 && !leaf))
      err = cb(n->item.key, n->item.value, level, opaque);
  }

  return err;
}

error trie_walk(const trie_t       *t,
                trie_walk_flags     flags,
                trie_walk_callback *cb,
                void               *opaque)
{
  error (*walker)(const trie__node_t *n,
                  trie_walk_flags     flags,
                  int                 level,
                  trie_walk_callback *cb,
                  void               *opaque);

  if (t == NULL)
    return error_OK;

  switch (flags & trie_WALK_ORDER_MASK)
  {
  default:
  case trie_WALK_IN_ORDER:
    walker = trie_walk_in_order;
    break;

  case trie_WALK_PRE_ORDER:
    walker = trie_walk_pre_order;
    break;

  case trie_WALK_POST_ORDER:
    walker = trie_walk_post_order;
    break;
  }

  return walker(t->root, flags, 0, cb, opaque);
}

/* ----------------------------------------------------------------------- */
