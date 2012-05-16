/* trie.c -- associative array implemented as trie */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memento.h"

#include "errors.h"
#include "utils.h"
#include "queue.h"
#include "string.h"
#include "string-kv.h"

#include "trie.h"
#include "trie-impl.h"

/* ----------------------------------------------------------------------- */

static trie__node_t *trie__node_create(trie_t     *t,
                                       const void *key,
                                       size_t      keylen,
                                       const void *value)
{
  trie__node_t *n;

  n = malloc(sizeof(*n));
  if (n == NULL)
    return NULL;

  n->child[0]    = NULL;
  n->child[1]    = NULL;
  n->item.key    = key;
  n->item.keylen = keylen;
  n->item.value  = value;

  t->count++;

  return n;
}

static void trie__node_destroy(trie_t *t, trie__node_t *n)
{
  if (t->destroy_key)
    t->destroy_key((void *) n->item.key); /* must cast away const */
  if (t->destroy_value)
    t->destroy_value((void *) n->item.value);

  free(n);

  t->count--;
}

/* ----------------------------------------------------------------------- */

/* NULL passed into the destroy callbacks signifies that these internal
 * routines should be used. They are setup to handle malloc'd strings.
 */

error trie_create(const void          *default_value,
                  trie_destroy_key    *destroy_key,
                  trie_destroy_value  *destroy_value,
                  trie_t             **pt)
{
  trie_t *t;

  *pt = NULL;

  t = malloc(sizeof(*t));
  if (t == NULL)
    return error_OOM;

  t->root          = NULL;
  t->default_value = default_value;

  /* if NULL is specified for callbacks then default to handlers suitable for
   * a malloc'd string */

  t->destroy_key   = destroy_key   ? destroy_key   : stringkv_destroy;
  t->destroy_value = destroy_value ? destroy_value : stringkv_destroy;

  t->count         = 0;

  *pt = t;

  return error_OK;
}

static error trie__destroy_node(trie__node_t *n, int level, void *opaque)
{
  NOT_USED(level);

  trie__node_destroy(opaque, n);

  return error_OK;
}

void trie_destroy(trie_t *t)
{
  (void) trie__walk_internal(t, trie_WALK_POST_ORDER, trie__destroy_node, t);

  free(t);
}

/* ----------------------------------------------------------------------- */

/* Extract the next binary direction from the key.
 * Within a byte the MSB is extacted first.
 */
#define GET_NEXT_DIR(DIR, KEY, KEYEND) \
do                                     \
{                                      \
  if ((depth++ & 7) == 0)              \
    c = (KEY == KEYEND) ? 0 : *KEY++;  \
                                       \
  DIR = (c & 0x80) != 0;               \
  c <<= 1;                             \
}                                      \
while (0)

/* ----------------------------------------------------------------------- */

const void *trie_lookup(trie_t *t, const void *key, size_t keylen)
{
  const unsigned char *ukey    = key;
  const unsigned char *ukeyend = ukey + keylen;
  int                  depth;
  const trie__node_t  *n;
  int                  dir;
  unsigned char        c = 0;

  depth = 0;

  for (n = t->root; n; n = n->child[dir])
  {
    if (IS_LEAF(n))
      break;

    GET_NEXT_DIR(dir, ukey, ukeyend);
  }

  if (n && n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0)
    return n->item.value; /* found */
  else
    return t->default_value; /* not found */
}

/* ----------------------------------------------------------------------- */

static trie__node_t *trie__insert_split(trie_t       *t,
                                        trie__node_t *m,
                                        trie__node_t *n,
                                        int           depth)
{
  trie__node_t        *x;
  const unsigned char *ukeym;
  const unsigned char *ukeymend;
  const unsigned char *ukeyn;
  const unsigned char *ukeynend;
  int                  mdir;
  int                  ndir;

  x = trie__node_create(t, NULL, 0L, NULL);
  if (x == NULL)
    return NULL; /* OOM */

  ukeym    = m->item.key;
  ukeymend = ukeym + m->item.keylen;

  mdir = 0;
  if (ukeym + (depth >> 3) < ukeymend)
    mdir = (ukeym[depth >> 3] >> (7 - (depth & 7))) & 1;

  ukeyn    = n->item.key;
  ukeynend = ukeyn + n->item.keylen;

  ndir = 0;
  if (ukeyn + (depth >> 3) < ukeynend)
    ndir = (ukeyn[depth >> 3] >> (7 - (depth & 7))) & 1;

  switch (mdir * 2 + ndir)
  {
  case 0:
    x->child[0] = trie__insert_split(t, m, n, depth + 1);
    break;
  case 3:
    x->child[1] = trie__insert_split(t, m, n, depth + 1);
    break;
  case 1:
    x->child[0] = m;
    x->child[1] = n;
    break;
  case 2:
    x->child[0] = n;
    x->child[1] = m;
    break;
  }

  // missing error handling when we return NULL to ourselves

  return x;
}

error trie_insert(trie_t     *t,
                  const void *key,
                  size_t      keylen,
                  const void *value)
{
  const unsigned char *ukey    = key;
  const unsigned char *ukeyend = ukey + keylen;
  int                  depth;
  trie__node_t       **pn;
  trie__node_t        *n;
  int                  dir;
  unsigned char        c = 0;
  trie__node_t        *m;

  /* search, but save the parent pointer too */

  depth = 0;

  for (pn = &t->root; (n = *pn); pn = &n->child[dir])
  {
    if (IS_LEAF(n))
      break;

    GET_NEXT_DIR(dir, ukey, ukeyend);
  }

  if (n && n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0)
    return error_EXISTS;

  m = trie__node_create(t, key, keylen, value);
  if (m == NULL)
    return error_OOM;

  if (n)
  {
    /* leaf node: need to split */

    trie__node_t *m2 = m;

    assert(IS_LEAF(n));

    m = trie__insert_split(t, m, n, depth);
    if (m == NULL)
    {
      trie__node_destroy(t, m2);
      return error_OOM;
    }
  }

  *pn = m;

  return error_OK;
}

/* ----------------------------------------------------------------------- */

/* To remove, we search for the specified node recursively. If we hit a leaf
 * node and it's the correct key then we detach the key from its parent and
 * return 1 meaning 'fix up the tree'. Otherwise we return -1 meaning 'not
 * found'.
 *
 * On return from recursion we then test the return code. If negative we
 * return that code immediately. If positive we examine the tree to see if
 * it needs fixing up. A tree with a left or right NULL subtree pointer where
 * the other direction is not a subtree is invalid, and needs fixing up.
 *
 * A tree with a valid structure can terminate immediately and returns -2.
 */
static int trie__remove_node(trie_t        *t,
                             trie__node_t **pn,
                             const void    *key,
                             size_t         keylen,
                             int            depth)
{
  const unsigned char *ukey    = key;
  const unsigned char *ukeyend = ukey + keylen;
  trie__node_t        *n = *pn;
  trie__node_t        *m;

  assert(n != NULL);

  if (IS_LEAF(n))
  {
    if (!(n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0))
      return -1; /* not found */

    m = NULL; /* delete link from parent */
  }
  else
  {
    int           dir;
    int           rc;
    trie__node_t *left, *right;
    int           leafleft, leafright;

    dir = 0;
    if (ukey + (depth >> 3) < ukeyend)
      dir = (ukey[depth >> 3] >> (7 - (depth & 7))) & 1;

    if ((rc = trie__remove_node(t, &n->child[dir], key, keylen, depth + 1)) <= 0)
      return rc;

    left  = n->child[0];
    right = n->child[1];

    leafleft  = (right == NULL) && IS_LEAF(left);
    leafright = (left  == NULL) && IS_LEAF(right);
    if (!leafleft && !leafright)
      return -2; /* subtree is in a valid state */

    m = leafleft ? left : right;
    assert(m);
  }

  trie__node_destroy(t, n);
  *pn = m;

  return 1;
}

void trie_remove(trie_t *t, const void *key, size_t keylen)
{
  (void) trie__remove_node(t, &t->root, key, keylen, 0);
}

/* ----------------------------------------------------------------------- */

typedef struct trie__select_args
{
  int     k;
  item_t *item;
}
trie__select_args_t;

static error trie__select_node(trie__node_t *n,
                               int           level,
                               void         *opaque)
{
  trie__select_args_t *args = opaque;

  NOT_USED(level);

  if (args->k-- == 0)
  {
    args->item = &n->item;
    return error_STOP_WALK;
  }

  return error_OK;
}

const item_t *trie_select(trie_t *t, int k)
{
  error               err;
  trie__select_args_t args;

  args.k    = k;
  args.item = NULL;

  err = trie__walk_internal(t,
                            trie_WALK_IN_ORDER | trie_WALK_LEAVES,
                            trie__select_node,
                            &args);

  /* no errors save for the expected ones should happen here */
  assert(err == error_OK || err == error_STOP_WALK);

  return args.item;
}

/* ----------------------------------------------------------------------- */

int trie_count(trie_t *t)
{
  return t->count;
}

/* ----------------------------------------------------------------------- */

/* This is similar to trie__walk_in_order, but returns items as an item_t
 * pointer. */
static error trie__lookup_prefix_walk(const trie__node_t  *n,
                                      trie_found_callback *cb,
                                      void                *opaque)
{
  error err;

  if (n == NULL)
    return error_OK;

  if (IS_LEAF(n))
  {
    return cb(&n->item, opaque);
  }
  else
  {
    err = trie__lookup_prefix_walk(n->child[0], cb, opaque);
    if (!err)
      err = trie__lookup_prefix_walk(n->child[1], cb, opaque);
    return err;
  }
}

error trie_lookup_prefix(const trie_t        *t,
                         const void          *prefix,
                         size_t               prefixlen, // bytes
                         trie_found_callback *cb,
                         void                *opaque)
{
  const unsigned char *uprefix    = prefix;
  const unsigned char *uprefixend = uprefix + prefixlen;
  int                  depth;
  trie__node_t        *n;
  unsigned char        c = 0;
  int                  dir;

  if (t->root == NULL)
    return error_OK; /* empty tree */

  depth = 0;

  for (n = t->root; n; n = n->child[dir])
  {
    if (IS_LEAF(n) || (size_t) depth == prefixlen * 8)
      break;

    GET_NEXT_DIR(dir, uprefix, uprefixend);
  }

  /* If we tried to walk in a direction not present in the trie then the
   * prefix can't exist in it. */
  if (n == NULL)
    return error_NOT_FOUND;

  if (IS_LEAF(n))
  {
    /* We've found a leaf which may or may not match. If it matches then we
     * can call the callback for it. */
    if (n->item.keylen >= prefixlen &&
        memcmp(prefix, n->item.key, prefixlen) == 0)
      return cb(&n->item, opaque);
    else
      return error_NOT_FOUND;
  }
  else
  {
    return trie__lookup_prefix_walk(n, cb, opaque);
  }
}

/* ----------------------------------------------------------------------- */

static error trie__walk_internal_in_order(trie__node_t                 *n,
                                          trie_walk_flags               flags,
                                          int                           level,
                                          trie__walk_internal_callback *cb,
                                          void                         *opaque)
{
  error err;

  if (n == NULL)
    return error_OK;

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

static error trie__walk_internal_pre_order(trie__node_t                 *n,
                                           trie_walk_flags               flags,
                                           int                           level,
                                           trie__walk_internal_callback *cb,
                                           void                         *opaque)
{
  error err;

  if (n == NULL)
    return error_OK;

  {
    int leaf;

    leaf = IS_LEAF(n);

    if (((flags & trie_WALK_LEAVES)   != 0 && leaf) ||
        ((flags & trie_WALK_BRANCHES) != 0 && !leaf))
      err = cb(n, level, opaque);
    else
      err = error_OK;
  }
  if (!err)
    err = trie__walk_internal_pre_order(n->child[0], flags, level + 1, cb, opaque);
  if (!err)
    err = trie__walk_internal_pre_order(n->child[1], flags, level + 1, cb, opaque);

  return err;
}

static error trie__walk_internal_post_order(trie__node_t                 *n,
                                            trie_walk_flags               flags,
                                            int                           level,
                                            trie__walk_internal_callback *cb,
                                            void                         *opaque)
{
  error err;

  if (n == NULL)
    return error_OK;

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

error trie__walk_internal(trie_t                       *t,
                          trie_walk_flags               flags,
                          trie__walk_internal_callback *cb,
                          void                         *opaque)
{
  error (*walker)(trie__node_t                 *n,
                  trie_walk_flags               flags,
                  int                           level,
                  trie__walk_internal_callback *cb,
                  void                         *opaque);

  if (t == NULL)
    return error_OK;

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

/* ----------------------------------------------------------------------- */

error trie__breadthwalk_internal(trie_t                       *t,
                                 trie_walk_flags               flags,
                                 trie__walk_internal_callback *cb,
                                 void                         *opaque)
{
  typedef struct nodedepth
  {
    trie__node_t *node;
    int           depth;
  }
  nodedepth;

  error     err;
  queue_t  *queue;
  nodedepth nd;

  if (t == NULL)
    return error_OK;

  queue = queue_create(100, sizeof(nodedepth));
  if (queue == NULL)
    return error_OOM;

  assert(t->root);

  nd.node  = t->root;
  nd.depth = 0;

  err = queue_enqueue(queue, &nd);
  if (err)
    return err;

  while (!queue_empty(queue))
  {
    int       leaf;
    nodedepth ndc;

    err = queue_dequeue(queue, &nd);
    if (err)
      return err;

    leaf = IS_LEAF(nd.node);

    // this actually means /visit/ leaves or branches we still walk
    // the entire structure and queue up child elements for visiting
    if (((flags & trie_WALK_LEAVES)   != 0 && leaf) ||
        ((flags & trie_WALK_BRANCHES) != 0 && !leaf))
    {
      err = cb(nd.node, nd.depth, opaque);
      if (err)
        return err;
    }

    ndc.depth = nd.depth + 1;

    if (nd.node->child[0])
    {
      ndc.node = nd.node->child[0];
      err = queue_enqueue(queue, &ndc);
      if (err)
        return err;
    }
    if (nd.node->child[1])
    {
      ndc.node = nd.node->child[1];
      err = queue_enqueue(queue, &ndc);
      if (err)
        return err;
    }
  }

  queue_destroy(queue);

  return error_OK;
}
