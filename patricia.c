/* patricia.c -- associative array implemented as patricia trie */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memento.h"

#include "queue.h"

#include "errors.h"
#include "utils.h"
#include "string.h"
#include "string-kv.h"

#include "patricia.h"
#include "patricia-impl.h"

/* ----------------------------------------------------------------------- */

static patricia__node_t *patricia__node_create(patricia_t *t,
                                               const void *key,
                                               size_t      keylen,
                                               const void *value)
{
  patricia__node_t *n;

  n = malloc(sizeof(*n));
  if (n == NULL)
    return NULL;

  n->child[0]    = NULL;
  n->child[1]    = NULL;
  n->item.key    = key;
  n->item.keylen = keylen;
  n->item.value  = value;
  n->bit         = 0xdeadbeef; /* expected to be overwritten subsequently */

  t->count++;

  return n;
}

static void patricia__node_clear(patricia_t *t, patricia__node_t *n)
{
  if (t->destroy_key && n->item.key)
    t->destroy_key((void *) n->item.key); /* must cast away const */
  if (t->destroy_value && n->item.value)
    t->destroy_value((void *) n->item.value);
}

static void patricia__node_destroy(patricia_t *t, patricia__node_t *n)
{
  patricia__node_clear(t, n);

  free(n);

  t->count--;
}

/* ----------------------------------------------------------------------- */

error patricia_create(const void              *default_value,
                      patricia_destroy_key    *destroy_key,
                      patricia_destroy_value  *destroy_value,
                      patricia_t             **pt)
{
  patricia_t *t;

  *pt = NULL;

  t = malloc(sizeof(*t));
  if (t == NULL)
    return error_OOM;

  /* the root node is only used for an all-zero-bits key */
  t->root = patricia__node_create(t, NULL, 0, NULL);
  if (t->root == NULL)
    return error_OOM;

  t->root->child[0] = t->root; /* left child points to self (for root node) */
  t->root->bit      = -1;

  t->default_value = default_value;

  /* if NULL is specified for callbacks then default to handlers suitable for
   * a malloc'd string */

  t->destroy_key   = destroy_key   ? destroy_key   : stringkv_destroy;
  t->destroy_value = destroy_value ? destroy_value : stringkv_destroy;

  t->count         = 0;

  *pt = t;

  return error_OK;
}

static error patricia__destroy_node(patricia__node_t *n,
                                    int               level,
                                    void             *opaque)
{
  NOT_USED(level);

  patricia__node_destroy(opaque, n);

  return error_OK;
}

void patricia_destroy(patricia_t *t)
{
  (void) patricia__walk_internal(t,
                                 patricia_WALK_POST_ORDER |
                                 patricia_WALK_BRANCHES,
                                 patricia__destroy_node,
                                 t);

  free(t);
}

/* ----------------------------------------------------------------------- */

/* Gets a byte or returns zero if it's out of range.
 * Copes with negative indices. */
// Have chosen to use size_t for 'unsigned ptrdiff_t'.
#define GET_BYTE(KEY, KEYEND, INDEX) \
  (((size_t) (INDEX) < (size_t) ((KEYEND) - (KEY))) ? (KEY)[INDEX] : 0)

/* Extract the specified indexed binary direction from the key. */
#define GET_DIR(KEY, KEYEND, INDEX) \
  (KEY ? (GET_BYTE(KEY, KEYEND, INDEX >> 3) & (1 << (7 - ((INDEX) & 7)))) != 0 : 0)

/* ----------------------------------------------------------------------- */

static int iszero(const void *k, size_t len)
{
  const unsigned char *start = k;
  const unsigned char *end   = start + len;

  while (start < end)
    if (*start++)
      return 0;

  return 1;
}

/* ----------------------------------------------------------------------- */

static INLINE const patricia__node_t *patricia__lookup(const patricia__node_t *n,
                                                       const void             *key,
                                                       size_t                  keylen)
{
  const unsigned char *ukey    = key;
  const unsigned char *ukeyend = ukey + keylen;
  int                  i;

  /* We follow nodes until we hit a lower bit value than previously
   * encountered. Sedgewick has values increasing as we progress through the
   * tree. Other texts describe it the other way around with the values
   * descending but that requires known key lengths. */

  do
  {
    i = n->bit;
    n = n->child[GET_DIR(ukey, ukeyend, i)];
    assert(n != NULL);
  }
  while (n->bit > i); /* we encounter ascending bit indices */

  return n;
}

const void *patricia_lookup(const patricia_t *t,
                            const void       *key,
                            size_t            keylen)
{
  patricia__node_t *n;

  assert(key != NULL);
  assert(keylen > 0);

  /* test for empty tree */
  n = t->root;
  if (n == NULL)
    return NULL;

  /* keys consisting of all zero bits always live in the root node */
  if (unlikely(iszero(key, keylen)))
    return n->item.value; /* found */

  n = patricia__lookup(n, key, keylen);

  assert(n != NULL);
  if (n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0)
    return n->item.value; /* found */
  else
    return t->default_value; /* not found */
}

/* ----------------------------------------------------------------------- */

error patricia_insert(patricia_t *t,
                      const void *key,
                      size_t      keylen,
                      const void *value)
{
  const unsigned char *ukey;
  const unsigned char *ukeyend;
  int                  bit;

  {
    patricia__node_t    *q;
    const unsigned char *qkey;
    const unsigned char *qkeyend;
    int                  i;

    q = t->root;
    assert(q != NULL);

    /* keys consisting of all zero bits always live in the root node */
    if (unlikely(iszero(key, keylen)))
      goto update;

    /* find closest node */
    q = (patricia__node_t *) patricia__lookup(q, key, keylen); /* we cast away const */
    assert(q != NULL);

    if (q->item.keylen == keylen && memcmp(q->item.key, key, keylen) == 0)
    {
update:
      if (q->item.key == key)
      {
        /* existing key - just update the value */
        q->item.value  = value;
      }
      else
      {
        patricia__node_clear(t, q);

        q->item.key    = key;
        q->item.keylen = keylen;
        q->item.value  = value;
      }

      return error_OK;
    }

    /* we've found a node which differs */

    qkey    = q->item.key;
    qkeyend = qkey + q->item.keylen;

    ukey    = key;
    ukeyend = ukey + keylen;

    /* locate the critical bit */

    bit = keydiffbit(qkey, qkeyend - qkey, ukey, ukeyend - ukey);
    // do i need the if (bit == -1) test?
  }

  /* insert new item */

  {
    patricia__node_t  *newnode;
    patricia__node_t  *n;
    int                nbit;
    int                parbit;
    patricia__node_t **pn;
    int                newdir;

    /* allocate new node */

    newnode = patricia__node_create(t, key, keylen, value);
    if (newnode == NULL)
      return error_OOM;

    n    = t->root;
    nbit = n->bit;
    do
    {
      parbit = nbit;
      pn     = &n->child[GET_DIR(ukey, ukeyend, nbit)];
      n      = *pn;
      nbit   = n->bit;
    }
    /* while (we've not hit the bit we want && encountered ascending) */
    while (nbit < bit && nbit > parbit);

    /* 'n' now points to a node with a bit index greater than our intended
     * index, or to the head node */

    newnode->bit = bit;

    /* the set bit indexes the direction we take to arrive at ourselves */
    newdir = GET_DIR(ukey, ukeyend, bit);
    assert(newdir == 0 || newdir == 1);

    newnode->child[newdir]  = newnode;
    newnode->child[!newdir] = n;

    *pn = newnode;
  }

  return error_OK;
}

/* ----------------------------------------------------------------------- */

void patricia_remove(patricia_t *t, const void *key, size_t keylen)
{
  /* NYI */
}

/* ----------------------------------------------------------------------- */

typedef struct patricia__select_args
{
  int     k;
  item_t *item;
}
patricia__select_args_t;

static error patricia__select_node(patricia__node_t *n,
                                   int               level,
                                   void             *opaque)
{
  patricia__select_args_t *args = opaque;

  NOT_USED(level);

  if (args->k-- == 0)
  {
    args->item = &n->item;
    return error_STOP_WALK;
  }

  return error_OK;
}

/* Walk the tree until the k'th leaf is encountered and return it. */
const item_t *patricia_select(patricia_t *t, int k)
{
  error                   err;
  patricia__select_args_t args;

  args.k    = k;
  args.item = NULL;

  err = patricia__walk_internal(t,
                                patricia_WALK_LEAVES,
                                patricia__select_node,
                                &args);

  /* no errors save for the expected ones should happen here */
  assert(err == error_OK || err == error_STOP_WALK);

  return args.item;
}

/* ----------------------------------------------------------------------- */

int patricia_count(patricia_t *t)
{
  return t->count;
}

/* ----------------------------------------------------------------------- */

/* This is similar to patricia__walk_in_order, but returns items as an item_t
 * pointer. */
static error patricia__lookup_prefix_walk(const patricia__node_t  *n,
                                          const void              *prefix,
                                          size_t                   prefixlen,
                                          patricia_found_callback *cb,
                                          void                    *opaque)
{
  error err;
  int   i;

  if (n == NULL)
    return error_OK;

  for (i = 0; i < 2; i++)
  {
    const patricia__node_t *c = n->child[i];

    if (c == NULL)
      continue;

    /* does this child point to a leaf? */
    if (c->bit <= n->bit)
    {
      // assert(c->item.keylen >= prefixlen);

      //if (c->item.keylen >= prefixlen &&
      //    memcmp(c->item.key, prefix, prefixlen) == 0)
      {
        err = cb(&c->item, opaque);
        if (err)
          return err;
      }
    }
    else /* branch */
    {
      err = patricia__lookup_prefix_walk(c, prefix, prefixlen, cb, opaque);
      if (err)
        return err;
    }
  }

  return error_OK;
}

error patricia_lookup_prefix(const patricia_t        *t,
                             const void              *prefix,
                             size_t                   prefixlen,
                             patricia_found_callback *cb,
                             void                    *opaque)
{
  const unsigned char *uprefix    = prefix;
  const unsigned char *uprefixend = uprefix + prefixlen;
  patricia__node_t    *n;
  patricia__node_t    *top;
  int                  i;

  n   = t->root;
  top = NULL;

  if (n == NULL)
    return error_OK; /* empty tree */

  /* The patricia trie tells us how to branch for a given bit but not what
   * the intermediate bits were. This means we can begin to search the trie
   * by prefix and end up in totally the wrong place. */

  /* search for the prefix as best as we can */
  do
  {
    i = n->bit;
    n = n->child[GET_DIR(uprefix, uprefixend, i)];
    assert(n != NULL);

    /* remember the root of the subtree which best matches the prefix */
    if (i < (int) prefixlen * 8) /* i can be negative */
      top = n;
  }
  while (n->bit > i); /* we encounter ascending bit indices */

  /* ensure the prefix exists */
  if (n->item.keylen < prefixlen ||
      memcmp(n->item.key, prefix, prefixlen) != 0)
    return error_NOT_FOUND;

  if (prefixlen * 8 - 1 == i /* not n->bit */)
  {
    // single exact match?
    return error_OK;
  }

  assert(top != NULL);

  return patricia__lookup_prefix_walk(top, prefix, prefixlen, cb, opaque);
}

/* ----------------------------------------------------------------------- */

static error patricia__walk_internal_in_order(patricia__node_t                 *n,
                                              patricia_walk_flags               flags,
                                              int                               level,
                                              patricia__walk_internal_callback *cb,
                                              void                             *opaque)
{
  error err;
  int   i;

  if (n == NULL)
    return error_OK;

  for (i = 0; i < 2; i++)
  {
    if (n->child[i] == NULL)
      continue;

    /* does this child point to a leaf? */
    if (n->child[i]->bit <= n->bit)
    {
      if ((flags & patricia_WALK_LEAVES) != 0)
      {
        err = cb(n->child[i], level, opaque);
        if (err)
          return err;
      }
    }
    else
    {
      err = patricia__walk_internal_in_order(n->child[i], flags, level + 1, cb, opaque);
      if (err)
        return err;
    }

    /* self */
    if (i == 0 && (flags & patricia_WALK_BRANCHES) != 0) /* inbetween */
    {
      err = cb(n, level, opaque);
      if (err)
        return err;
    }
  }

  return error_OK;
}

static error patricia__walk_internal_pre_order(patricia__node_t                 *n,
                                               patricia_walk_flags               flags,
                                               int                               level,
                                               patricia__walk_internal_callback *cb,
                                               void                             *opaque)
{
  error err;
  int   i;

  if (n == NULL)
    return error_OK;

  /* self */
  if ((flags & patricia_WALK_BRANCHES) != 0)
  {
    err = cb(n, level, opaque);
    if (err)
      return err;
  }

  for (i = 0; i < 2; i++)
  {
    if (n->child[i] == NULL)
      continue;

    /* does this child point to a leaf? */
    if (n->child[i]->bit <= n->bit)
    {
      if ((flags & patricia_WALK_LEAVES) != 0)
      {
        err = cb(n->child[i], level, opaque);
        if (err)
          return err;
      }
    }
    else
    {
      err = patricia__walk_internal_pre_order(n->child[i], flags, level + 1, cb, opaque);
      if (err)
        return err;
    }
  }

  return error_OK;
}

static error patricia__walk_internal_post_order(patricia__node_t                 *n,
                                                patricia_walk_flags               flags,
                                                int                               level,
                                                patricia__walk_internal_callback *cb,
                                                void                             *opaque)
{
  error err;
  int   i;

  if (n == NULL)
    return error_OK;

  for (i = 0; i < 2; i++)
  {
    if (n->child[i] == NULL)
      continue;

    /* does this child point to a leaf? */
    if (n->child[i]->bit <= n->bit)
    {
      if ((flags & patricia_WALK_LEAVES) != 0)
      {
        err = cb(n->child[i], level, opaque);
        if (err)
          return err;
      }
    }
    else
    {
      err = patricia__walk_internal_post_order(n->child[i], flags, level + 1, cb, opaque);
      if (err)
        return err;
    }
  }

  /* self */
  if ((flags & patricia_WALK_BRANCHES) != 0)
  {
    err = cb(n, level, opaque);
    if (err)
      return err;
  }

  return error_OK;
}

error patricia__walk_internal(patricia_t                       *t,
                              patricia_walk_flags               flags,
                              patricia__walk_internal_callback *cb,
                              void                             *opaque)
{
  error (*walker)(patricia__node_t                 *n,
                  patricia_walk_flags               flags,
                  int                               level,
                  patricia__walk_internal_callback *cb,
                  void                             *opaque);

  if (t == NULL)
    return error_OK;

  switch (flags & patricia_WALK_ORDER_MASK)
  {
  default:
  case patricia_WALK_IN_ORDER:
    walker = patricia__walk_internal_in_order;
    break;

  case patricia_WALK_PRE_ORDER:
    walker = patricia__walk_internal_pre_order;
    break;

  case patricia_WALK_POST_ORDER:
    walker = patricia__walk_internal_post_order;
    break;
  }

  return walker(t->root, flags, 0, cb, opaque);
}

