/* critbit.c -- associative array implemented as critbit tree */

// this code based on bernstein crit-bits via imperialviolet.org paper
// http://www.imperialviolet.org/2008/09/29/critbit-trees.html

/* We assume that returned malloc blocks are aligned to at least a two-byte
 * boundary, leaving us the bottom bit spare to use as as a node type flag.
 */

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

#include "critbit.h"
#include "critbit-impl.h"

/* ----------------------------------------------------------------------- */

static critbit__extnode_t *critbit__extnode_create(critbit_t  *t,
                                                   const void *key,
                                                   size_t      keylen,
                                                   const void *value)
{
  critbit__extnode_t *n;

  n = malloc(sizeof(*n));
  if (n == NULL)
    return NULL;

  n->item.key    = key;
  n->item.keylen = keylen;
  n->item.value  = value;

  t->extcount++;

  return n;
}

static void critbit__extnode_clear(critbit_t *t, critbit__extnode_t *n)
{
  if (t->destroy_key && n->item.key)
    t->destroy_key((void *) n->item.key); /* must cast away const */
  if (t->destroy_value && n->item.value)
    t->destroy_value((void *) n->item.value);
}

static void critbit__extnode_destroy(critbit_t *t, critbit__extnode_t *n)
{
  critbit__extnode_clear(t, n);

  free(n);

  t->extcount--;
}

/* ----------------------------------------------------------------------- */

static critbit__node_t *critbit__node_create(critbit_t *t,
                                             int        byte,
                                             uint8_t    otherbits)
{
  critbit__node_t *n;

  n = malloc(sizeof(*n));
  if (n == NULL)
    return NULL;

  n->child[0]  = NULL;
  n->child[1]  = NULL;
  n->byte      = byte;
  n->otherbits = otherbits;

  t->intcount++;

  return n;
}

static void critbit__node_destroy(critbit_t *t, critbit__node_t *n)
{
  free(n);

  t->intcount--;
}

/* ----------------------------------------------------------------------- */

error critbit_create(const void             *default_value,
                     critbit_destroy_key    *destroy_key,
                     critbit_destroy_value  *destroy_value,
                     critbit_t             **pt)
{
  critbit_t *t;

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

  t->intcount      = 0;
  t->extcount      = 0;

  *pt = t;

  return error_OK;
}

static error critbit__destroy_node(critbit__node_t *n,
                                   int              level,
                                   void            *opaque)
{
  NOT_USED(level);

  if (IS_EXTERNAL(n))
    critbit__extnode_destroy(opaque, FROM_STORE(n));
  else
    critbit__node_destroy(opaque, n);

  return error_OK;
}

void critbit_destroy(critbit_t *t)
{
  (void) critbit__walk_internal(t,
                                critbit_WALK_POST_ORDER |
                                critbit_WALK_LEAVES     |
                                critbit_WALK_BRANCHES,
                                critbit__destroy_node,
                                t);

  free(t);
}

/* ----------------------------------------------------------------------- */

/* Gets a byte or returns zero if it's out of range. */
#define GET_BYTE(KEY, KEYEND, INDEX) \
  (((KEY) + (INDEX) < (KEYEND)) ? (KEY)[INDEX] : 0)

/* Extract the specified indexed binary direction from the key. */
#define GET_DIR(KEY, KEYEND, INDEX, OTHERBITS) \
  ((1 + ((OTHERBITS) | GET_BYTE(KEY, KEYEND, INDEX))) >> 8)

/* ----------------------------------------------------------------------- */

static INLINE const critbit__extnode_t *critbit__lookup(const critbit__node_t *n,
                                                        const void            *key,
                                                        size_t                 keylen)
{
  const unsigned char *ukey    = key;
  const unsigned char *ukeyend = ukey + keylen;
  int                  dir;

  for (; IS_INTERNAL(n); n = n->child[dir])
    dir = GET_DIR(ukey, ukeyend, n->byte, n->otherbits);

  return FROM_STORE(n);
}

const void *critbit_lookup(const critbit_t *t,
                           const void      *key,
                           size_t           keylen)
{
  const critbit__extnode_t *n;

  assert(key != NULL);
  assert(keylen > 0);

  /* test for empty tree */
  if (t->root == NULL)
    return NULL;

  n = critbit__lookup(t->root, key, keylen);

  assert(n != NULL);
  if (n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0)
    return n->item.value; /* found */
  else
    return t->default_value; /* not found */
}

/* ----------------------------------------------------------------------- */

// Can we insert an all-zero-bits key into an empty critbit tree?
// - Which is the critbit in that case?
// - It turns out that's not a relevant question for crit-bit as the only
// node which needs to be created is an external node.
// ...
// - But for patricia there are no external nodes. /Everything/ must have a
// crit-bit, but you can't discover the crit-bit of an all-zero-bits key
// unless you arbitrarily fix the length of the keys.

error critbit_insert(critbit_t  *t,
                     const void *key,
                     size_t      keylen,
                     const void *value)
{
  const unsigned char *ukey;
  const unsigned char *ukeyend;
  int                  newbyte;
  unsigned int         newotherbits;
  const unsigned char *qkey;
  const unsigned char *qkeyend;

  {
    critbit__extnode_t *q;

    /* deal with inserting into an empty tree */
    if (t->root == NULL)
    {
      critbit__extnode_t *newextnode;

      newextnode = critbit__extnode_create(t, key, keylen, value);
      if (newextnode == NULL)
        return error_OOM;

      t->root = TO_STORE(newextnode);

      return error_OK;
    }

    /* find closest node */
    q = (critbit__extnode_t *) critbit__lookup(t->root, key, keylen); /* we cast away const */
    assert(q != NULL);

    if (q->item.keylen == keylen && memcmp(q->item.key, key, keylen) == 0)
    {
      /* update the existing key's value */

      if (q->item.key == key)
      {
        /* existing key - just update the value */
        q->item.value  = value;
      }
      else
      {
        critbit__extnode_clear(t, q);

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

    {
      int nbit;

      nbit = keydiffbit(qkey, qkeyend - qkey, ukey, ukeyend - ukey);
      if (nbit == -1)
        return error_CLASHES;

      newotherbits = 1 << (7 - (nbit & 0x07));
      newotherbits ^= 255;

      newbyte = nbit >> 3;
    }
  }

  /* insert new item */

  {
    critbit__extnode_t *newextnode;
    critbit__node_t    *newnode;
    critbit__node_t   **pn;
    int                 newdir;

    /* allocate new external node */

    newextnode = critbit__extnode_create(t, key, keylen, value);
    if (newextnode == NULL)
      return error_OOM;

    /* allocate new internal node */

    newnode = critbit__node_create(t, newbyte, (uint8_t) newotherbits);
    if (newnode == NULL)
    {
      critbit__extnode_destroy(t, newextnode);
      return error_OOM;
    }

    /* insert new node */

    pn = &t->root;

    for (;;)
    {
      critbit__node_t *n;

      n = *pn;

      if (IS_EXTERNAL(n)    ||
          n->byte > newbyte ||
          (n->byte == newbyte && n->otherbits > newotherbits))
        break;

      pn = &n->child[GET_DIR(ukey, ukeyend, n->byte, n->otherbits)];
    }

    newdir = GET_DIR(qkey, qkeyend, newbyte, newotherbits);
    assert(newdir == 0 || newdir == 1);

    newnode->child[newdir] = *pn;
    newnode->child[!newdir] = TO_STORE(newextnode);

    *pn = newnode;
  }

  return error_OK;
}

/* ----------------------------------------------------------------------- */

void critbit_remove(critbit_t *t, const void *key, size_t keylen)
{
  const unsigned char *ukey    = key;
  const unsigned char *ukeyend = ukey + keylen;
  critbit__node_t     *n;
  critbit__node_t    **wherem;
  critbit__node_t    **wheren;
  int                  dir;
  critbit__extnode_t  *e;
  critbit__node_t     *lastn;

  n = t->root;

  wherem = NULL;
  wheren = &t->root;

  if (t->root == NULL)
    return; /* empty tree */

  while (IS_INTERNAL(n))
  {
    wherem = wheren;

    dir = GET_DIR(ukey, ukeyend, n->byte, n->otherbits);
    wheren = &n->child[dir];

    lastn = n;
    n = *wheren;
  }

  e = FROM_STORE(n);

  if (!(e->item.keylen == keylen && memcmp(e->item.key, key, keylen) == 0))
    return; /* not found */

  critbit__extnode_destroy(t, e);

  if (wherem == NULL)
  {
    t->root = NULL;
  }
  else
  {
    *wherem = lastn->child[1 - dir];

    critbit__node_destroy(t, lastn);
  }
}

/* ----------------------------------------------------------------------- */

int critbit_count(critbit_t *t)
{
  return t->intcount + t->extcount;
}

/* ----------------------------------------------------------------------- */

/* This is similar to critbit__walk_in_order, but returns items as an item_t
 * pointer. */
static error critbit__lookup_prefix_walk(const critbit__node_t  *n,
                                         critbit_found_callback *cb,
                                         void                   *opaque)
{
  error               err;
  critbit__extnode_t *e;

  if (IS_EXTERNAL(n))
  {
    e = FROM_STORE(n);

    err = cb(&e->item, opaque);
    if (err)
      return err;
  }
  else
  {
    err = critbit__lookup_prefix_walk(n->child[0], cb, opaque);
    if (err)
      return err;

    err = critbit__lookup_prefix_walk(n->child[1], cb, opaque);
    if (err)
      return err;
  }

  return error_OK;
}

error critbit_lookup_prefix(const critbit_t        *t,
                            const void             *prefix,
                            size_t                  prefixlen,
                            critbit_found_callback *cb,
                            void                   *opaque)
{
  const unsigned char   *uprefix    = prefix;
  const unsigned char   *uprefixend = uprefix + prefixlen;
  critbit__node_t       *n;
  int                    dir;
  const critbit__node_t *top;
  critbit__extnode_t    *e;

  n   = t->root;
  top = n;

  if (n == NULL)
    return error_OK; /* empty tree */

  while (IS_INTERNAL(n))
  {
    critbit__node_t *m;

    dir = GET_DIR(uprefix, uprefixend, n->byte, n->otherbits);

    m = n->child[dir];

    if ((size_t) n->byte < prefixlen)
      top = m;

    n = m;
  }

  e = FROM_STORE(n);

  /* ensure the prefix exists */
  if (e->item.keylen < prefixlen ||
      memcmp(e->item.key, prefix, prefixlen) != 0)
    return error_NOT_FOUND;

  assert(top != NULL);

  return critbit__lookup_prefix_walk(top, cb, opaque);
}

/* ----------------------------------------------------------------------- */

// unlike the walking methods in other data structures, we cannot callback on
// the current node as any non-leaf node does not hold an item
//
static error critbit__walk_internal_in_order(critbit__node_t                 *n,
                                             critbit_walk_flags               flags,
                                             int                              level,
                                             critbit__walk_internal_callback *cb,
                                             void                            *opaque)
{
  error err;
  int   i;

  if (n == NULL)
    return error_OK;

  if (IS_EXTERNAL(n))
  {
    if ((flags & critbit_WALK_LEAVES) != 0)
    {
      err = cb(n, level, opaque);
      if (err)
        return err;
    }
  }
  else
  {
    for (i = 0; i < 2; i++)
    {
      err = critbit__walk_internal_in_order(n->child[i], flags, level + 1, cb, opaque);
      if (err)
        return err;

      if (i == 0 && (flags & critbit_WALK_BRANCHES) != 0) /* inbetween */
      {
        err = cb(n, level, opaque);
        if (err)
          return err;
      }
    }
  }

  return error_OK;
}

static error critbit__walk_internal_pre_order(critbit__node_t                 *n,
                                              critbit_walk_flags               flags,
                                              int                              level,
                                              critbit__walk_internal_callback *cb,
                                              void                            *opaque)
{
  error err;
  int   i;

  if (n == NULL)
    return error_OK;

  if (IS_EXTERNAL(n))
  {
    if ((flags & critbit_WALK_LEAVES) != 0)
    {
      err = cb(n, level, opaque);
      if (err)
        return err;
    }
  }
  else
  {
    /* self */
    if ((flags & critbit_WALK_BRANCHES) != 0)
    {
      err = cb(n, level, opaque);
      if (err)
        return err;
    }

    for (i = 0; i < 2; i++)
    {
      err = critbit__walk_internal_pre_order(n->child[i], flags, level + 1, cb, opaque);
      if (err)
        return err;
    }
  }

  return error_OK;
}

static error critbit__walk_internal_post_order(critbit__node_t                 *n,
                                               critbit_walk_flags               flags,
                                               int                              level,
                                               critbit__walk_internal_callback *cb,
                                               void                            *opaque)
{
  error err;
  int   i;

  if (n == NULL)
    return error_OK;

  if (IS_EXTERNAL(n))
  {
    if ((flags & critbit_WALK_LEAVES) != 0)
    {
      err = cb(n, level, opaque);
      if (err)
        return err;
    }
  }
  else
  {
    for (i = 0; i < 2; i++)
    {
      err = critbit__walk_internal_post_order(n->child[i], flags, level + 1, cb, opaque);
      if (err)
        return err;
    }

    /* self */
    if ((flags & critbit_WALK_BRANCHES) != 0)
    {
      err = cb(n, level, opaque);
      if (err)
        return err;
    }
  }

  return error_OK;
}

error critbit__walk_internal(critbit_t                       *t,
                             critbit_walk_flags               flags,
                             critbit__walk_internal_callback *cb,
                             void                            *opaque)
{
  error (*walker)(critbit__node_t                 *n,
                  critbit_walk_flags               flags,
                  int                              level,
                  critbit__walk_internal_callback *cb,
                  void                            *opaque);

  if (t == NULL)
    return error_OK;

  switch (flags & critbit_WALK_ORDER_MASK)
  {
  default:
  case critbit_WALK_IN_ORDER:
    walker = critbit__walk_internal_in_order;
    break;

  case critbit_WALK_PRE_ORDER:
    walker = critbit__walk_internal_pre_order;
    break;

  case critbit_WALK_POST_ORDER:
    walker = critbit__walk_internal_post_order;
    break;
  }

  return walker(t->root, flags, 0, cb, opaque);
}

