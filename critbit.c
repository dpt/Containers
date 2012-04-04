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

static void critbit__extnode_destroy(critbit_t *t, critbit__extnode_t *n)
{
  if (t->destroy_key)
    t->destroy_key((void *) n->item.key); /* must cast away const */
  if (t->destroy_value)
    t->destroy_value((void *) n->item.value);

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

/* Extract the next binary direction from the key. */
#define GET_NEXT_DIR(KEY, KEYEND, INDEX, OTHERBITS) \
  ((1 + ((OTHERBITS) | GET_BYTE(KEY, KEYEND, INDEX))) >> 8)

/* ----------------------------------------------------------------------- */

static const critbit__extnode_t *critbit__lookup(const critbit__node_t *n,
                                                 const void            *key,
                                                 size_t                 keylen)
{
  const unsigned char *ukey    = key;
  const unsigned char *ukeyend = ukey + keylen;
  int                  dir;

  for (; IS_INTERNAL(n); n = n->child[dir])
    dir = GET_NEXT_DIR(ukey, ukeyend, n->byte, n->otherbits);

  return FROM_STORE(n);
}

const void *critbit_lookup(const critbit_t *t,
                           const void      *key,
                           size_t           keylen)
{
  const critbit__extnode_t *n;

  /* test for empty tree */
  if (t->root == NULL)
    return NULL;

  n = critbit__lookup(t->root, key, keylen);

  if (n && n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0)
    return n->item.value; /* found */
  else
    return t->default_value; /* not found */
}

/* ----------------------------------------------------------------------- */

error critbit_insert(critbit_t  *t,
                     const void *key,
                     size_t      keylen,
                     const void *value)
{
  const unsigned char *ukey    = key;
  const unsigned char *ukeyend = ukey + keylen;

  const unsigned char *qkey; // reorder these var defns
  const unsigned char *qkeyend;

  critbit__extnode_t  *newextnode;
  critbit__extnode_t  *q;
  int                  newbyte;
  uint32_t             newotherbits;
  critbit__node_t     *newnode;
  int                  newdir;
  critbit__node_t    **wherep;
  critbit__node_t     *p;

  /* deal with inserting into an empty tree */
  if (t->root == NULL)
  {
    newextnode = critbit__extnode_create(t, key, keylen, value);
    if (newextnode == NULL)
      return error_OOM;

    t->root = TO_STORE(newextnode);

    return error_OK;
  }

  /* lookup */
  q = (critbit__extnode_t *) critbit__lookup(t->root, key, keylen); // const -> nonconst badness

  /* find the critical bit */

  qkey    = q->item.key;
  qkeyend = qkey + q->item.keylen;

  /* find differing byte */
  for (newbyte = 0; newbyte < (int) keylen; newbyte++)
  {
    uint32_t a, b;

    a = GET_BYTE(qkey, qkeyend, newbyte);
    b = GET_BYTE(ukey, ukeyend, newbyte);

    if ((newotherbits = a ^ b) != 0)
      break;
  }

  /* if we hit end of key, see if we hit end of qkey too */
  // ought to indicate end of key another way (zero assumes a NUL terminated string),
  // but that conflicts with the right to return zero bits trailing at end of key ... can i not just compare both lengths here?
  if (newbyte == (int) keylen && (newotherbits = GET_BYTE(qkey, qkeyend, newbyte)) == 0)
    //if (newbyte == keylen && (newotherbits = qkey[newbyte]) == '\0')
    return error_EXISTS;

  /* find differing bit */
  /* spread the MSB right (three times is enough for a byte) */
  newotherbits |= newotherbits >> 1;
  newotherbits |= newotherbits >> 2;
  newotherbits |= newotherbits >> 4;
  /* wipe out all bits save for the MSB */
  newotherbits = newotherbits & ~(newotherbits >> 1);
  /* form a mask */
  newotherbits ^= 255;

  /* insert new item */

  newextnode = critbit__extnode_create(t, key, keylen, value);
  if (newextnode == NULL)
    return error_OOM;

  /* allocate new node structure */

  newnode = critbit__node_create(t, newbyte, (uint8_t) newotherbits);
  if (newnode == NULL)
  {
    critbit__extnode_destroy(t, newextnode);
    return error_OOM;
  }

  newdir = GET_NEXT_DIR(qkey, qkeyend, newbyte, newotherbits);

  newnode->child[1 - newdir] = TO_STORE(newextnode);

  /* insert new node */

  wherep = &t->root;

  for (;;)
  {
    p = *wherep;

    if (IS_EXTERNAL(p)    ||
        p->byte > newbyte ||
        (p->byte == newbyte && p->otherbits > newotherbits))
      break;

    wherep = &p->child[GET_NEXT_DIR(ukey, ukeyend, p->byte, p->otherbits)];
  }

  newnode->child[newdir] = *wherep;
  *wherep = newnode;

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

    dir = GET_NEXT_DIR(ukey, ukeyend, n->byte, n->otherbits);
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

typedef struct critbit__select_args
{
  int     k;
  item_t *item;
}
critbit__select_args_t;

static error critbit__select_node(critbit__node_t *n,
                                  int              level,
                                  void            *opaque)
{
  critbit__select_args_t *args = opaque;

  NOT_USED(level);

  if (args->k-- == 0)
  {
    critbit__extnode_t *e;

    e = FROM_STORE(n);

    args->item = &e->item;
    return error_STOP_WALK;
  }

  return error_OK;
}

const item_t *critbit_select(critbit_t *t, int k)
{
  error                  err;
  critbit__select_args_t args;

  args.k    = k;
  args.item = NULL;

  err = critbit__walk_internal(t,
                               critbit_WALK_LEAVES,
                               critbit__select_node,
                              &args);

  /* no errors save for the expected ones should happen here */
  assert(err == error_OK || err == error_STOP_WALK);

  return args.item;
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
  int                    i;
  const unsigned char   *ukey;

  n   = t->root;
  top = n;

  if (t->root == NULL)
    return error_OK; /* empty tree */

  while (IS_INTERNAL(n))
  {
    critbit__node_t *m;

    dir = GET_NEXT_DIR(uprefix, uprefixend, n->byte, n->otherbits);

    m = n->child[dir];

    if ((size_t) n->byte < prefixlen)
      top = m;

    n = m;
  }

  e = FROM_STORE(n);

  /* check the prefix exists */
  if (e->item.keylen < prefixlen)
    return error_NOT_FOUND;

  ukey = e->item.key;

  for (i = 0; i < (int) prefixlen; i++)
    if (uprefix[i] != ukey[i])
      return error_NOT_FOUND;

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
    err = critbit__walk_internal_in_order(n->child[0], flags, level + 1, cb, opaque);
    if (err)
      return err;

    if ((flags & critbit_WALK_BRANCHES) != 0)
    {
      err = cb(n, level, opaque); // it seems like we need two callback types...
      if (err)
        return err;
    }

    err = critbit__walk_internal_in_order(n->child[1], flags, level + 1, cb, opaque);
    if (err)
      return err;
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
    if ((flags & critbit_WALK_BRANCHES) != 0)
    {
      err = cb(n, level, opaque); // it seems like we need two callback types...
      if (err)
        return err;
    }

    err = critbit__walk_internal_pre_order(n->child[0], flags, level + 1, cb, opaque);
    if (err)
      return err;

    err = critbit__walk_internal_pre_order(n->child[1], flags, level + 1, cb, opaque);
    if (err)
      return err;
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
    err = critbit__walk_internal_post_order(n->child[0], flags, level + 1, cb, opaque);
    if (err)
      return err;

    err = critbit__walk_internal_post_order(n->child[1], flags, level + 1, cb, opaque);
    if (err)
      return err;

    if ((flags & critbit_WALK_BRANCHES) != 0)
    {
      err = cb(n, level, opaque); // it seems like we need two callback types...
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

/* ----------------------------------------------------------------------- */

error critbit__breadthwalk_internal(critbit_t                       *t,
                                    critbit_walk_flags               flags,
                                    critbit__walk_internal_callback *cb,
                                    void                            *opaque)
{
  typedef struct nodedepth
  {
    critbit__node_t *node;
    int              depth;
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
    if (((flags & critbit_WALK_LEAVES)   != 0 && leaf) ||
        ((flags & critbit_WALK_BRANCHES) != 0 && !leaf))
    {
      err = cb(nd.node, nd.depth, opaque);
      if (err)
        return err;
    }

    if (leaf)
      continue;

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
