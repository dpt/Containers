/* dstree.c -- associative array implemented as digital search tree */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base/memento/memento.h"

#include "datastruct/queue.h"

#include "base/errors.h"
#include "utils/utils.h"
#include "string.h"
#include "keyval/string.h"

#include "datastruct/dstree.h"

#include "impl.h"

/* ----------------------------------------------------------------------- */

static dstree__node_t *dstree__node_create(dstree_t   *t,
                                           const void *key,
                                           const void *value,
                                           size_t      keylen)
{
  dstree__node_t *n;

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

static void dstree__node_destroy(dstree_t *t, dstree__node_t *n)
{
  if (t->destroy_key)
    t->destroy_key((void *) n->item.key); /* must cast away const */
  if (t->destroy_value)
    t->destroy_value((void *) n->item.value);

  free(n);

  t->count--;
}

/* ----------------------------------------------------------------------- */

error dstree_create(const void            *default_value,
                    dstree_destroy_key    *destroy_key,
                    dstree_destroy_value  *destroy_value,
                    dstree_t             **pt)
{
  dstree_t *t;

  *pt = NULL;

  t = malloc(sizeof(*t));
  if (t == NULL)
    return error_OOM;

  t->root          = NULL;
  t->default_value = default_value;
  t->destroy_key   = destroy_key;
  t->destroy_value = destroy_value;

  t->count         = 0;

  *pt = t;

  return error_OK;
}

static error dstree__destroy_node(dstree__node_t *n, int level, void *opaque)
{
  NOT_USED(level);

  dstree__node_destroy(opaque, n);

  return error_OK;
}

void dstree_destroy(dstree_t *t)
{
  (void) dstree__walk_internal_post(t, dstree__destroy_node, t);

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

const void *dstree_lookup(dstree_t *t, const void *key, size_t keylen)
{
  const unsigned char  *ukey    = key;
  const unsigned char  *ukeyend = ukey + keylen;
  int                   depth;
  const dstree__node_t *n;
  int                   dir;
  unsigned char         c = 0;

  depth = 0;

  for (n = t->root; n; n = n->child[dir])
  {
    if (n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0)
      return n->item.value; /* found */

    GET_NEXT_DIR(dir, ukey, ukeyend);
  }

  return t->default_value; /* not found */
}

/* ----------------------------------------------------------------------- */

error dstree_insert(dstree_t   *t,
                    const void *key,
                    size_t      keylen,
                    const void *value)
{
  const unsigned char *ukey    = key;
  const unsigned char *ukeyend = ukey + keylen;
  int                  depth;
  dstree__node_t     **pn;
  dstree__node_t      *n;
  int                  dir;
  unsigned char        c = 0;

  depth = 0;

  for (pn = &t->root; (n = *pn); pn = &n->child[dir])
  {
    if (n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0)
      return error_EXISTS;

    GET_NEXT_DIR(dir, ukey, ukeyend);
  }

  *pn = dstree__node_create(t, key, value, keylen);
  if (*pn == NULL)
    return error_OOM;

  return error_OK;
}

/* ----------------------------------------------------------------------- */

void dstree_remove(dstree_t *t, const void *key, size_t keylen)
{
  const unsigned char *ukey    = key;
  const unsigned char *ukeyend = ukey + keylen;
  int                  depth;
  dstree__node_t     **pn;
  dstree__node_t      *n;
  int                  dir;
  unsigned char        c;
  dstree__node_t     **pm;
  dstree__node_t      *m;

  depth = 0;
  c     = 0;

  for (pn = &t->root; (n = *pn); pn = &n->child[dir])
  {
    if (n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0)
      break; /* found */

    GET_NEXT_DIR(dir, ukey, ukeyend);
  }

  if (n == NULL)
    return; /* doesn't exist */

  /* pn now points to pointer to the doomed node */

  /* if the doomed node is a leaf node we just delete the parent pointer */
  if (IS_LEAF(n))
  {
    *pn = NULL;
    dstree__node_destroy(t, n);
    return;
  }

  /* otherwise we look for a leaf node that we can swap into the place of the
   * parent node */

  m = n;

  /* we need any leaf node downward of the to-be-removed node. so we just set
   * depth to zero here and hope that the binary values GET_NEXT_DIR()
   * returns cut us a random path through the tree. */
  ukey  = key;
  depth = 0;

  do
  {
    /* if there is no right child, then go left.
     * if there is no left child, then go right.
     * if both are present, fetch a 'random' value from GET_NEXT_DIR() and
     * choose. this means that we only call it when necessary. */

    if (m->child[1] == NULL)
      pm = &m->child[0];
    else if (m->child[0] == NULL)
      pm = &m->child[1];
    else
    {
      GET_NEXT_DIR(dir, ukey, ukeyend);
      pm = &m->child[dir];
      depth--; /* GET_NEXT_DIR increments depth, which we must compensate for */
    }

    m = *pm;
    depth++;
  }
  while (!IS_LEAF(m));

  /* found leaf node */

  *pm = NULL; /* detach */
  m->child[0] = n->child[0];
  m->child[1] = n->child[1];
  *pn = m; /* reattach */

  dstree__node_destroy(t, n);
}

/* ----------------------------------------------------------------------- */

void dstree_remove2(dstree_t *t, const void *key, size_t keylen)
{
  const unsigned char *ukey    = key;
  const unsigned char *ukeyend = ukey + keylen;
  int                  depth;
  dstree__node_t     **pn;
  dstree__node_t      *n;
  unsigned char        c = 0;
  int                  dir;
  dstree__node_t      *root;
  dstree__node_t      *side;

  depth = 0;

  for (pn = &t->root; (n = *pn); pn = &n->child[dir])
  {
    if (n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0)
      break; /* found */

    GET_NEXT_DIR(dir, ukey, ukeyend);
  }

  if (n == NULL)
    return; /* not found */

  /* pick a random child. if that child's absent then pick the other */
  GET_NEXT_DIR(dir, ukey, ukeyend);
  if (n->child[dir] == NULL)
    dir = !dir;

  root = n->child[dir];  /* new tree root */
  side = n->child[!dir]; /* detached other side */

  /* detach root node */
  *pn = NULL;

  while (root)
  {
    int             newdir;
    dstree__node_t *nextroot, *nextside;

    GET_NEXT_DIR(newdir, ukey, ukeyend);
    if (n->child[newdir] == NULL)
      newdir = !newdir;

    nextroot = root->child[newdir];
    nextside = root->child[!newdir];

    root->child[dir]  = nextroot;
    root->child[!dir] = side;

    dir = newdir;
    root = nextroot;
    side = nextside;
  }

  dstree__node_destroy(t, n);
}

/* ----------------------------------------------------------------------- */

int dstree_count(dstree_t *t)
{
  return t->count;
}

/* ----------------------------------------------------------------------- */

typedef struct dstree_lookup_prefix_args
{
  const unsigned char   *uprefix;
  size_t                 prefixlen;
  dstree_found_callback *cb;
  void                  *opaque;
}
dstree_lookup_prefix_args_t;

static error dstree__lookup_prefix_node(dstree__node_t *n,
                                        int             level,
                                        void           *opaque)
{
  dstree_lookup_prefix_args_t *args = opaque;
  size_t                       prefixlen;

  NOT_USED(level);

  prefixlen = args->prefixlen;

  if (n->item.keylen >= prefixlen &&
      memcmp(n->item.key, args->uprefix, prefixlen) == 0)
  {
    return args->cb(&n->item, args->opaque);
  }
  else
  {
    return error_OK;
  }
}

error dstree_lookup_prefix(const dstree_t        *t,
                           const void            *prefix,
                           size_t                 prefixlen,
                           dstree_found_callback *cb,
                           void                  *opaque)
{
  error                       err;
  const unsigned char        *uprefix    = prefix;
  const unsigned char        *uprefixend = uprefix + prefixlen;
  int                         depth;
  const dstree__node_t       *n;
  unsigned char               c = 0;
  int                         dir;
  dstree_lookup_prefix_args_t args;
  int                         i;

  /* For a prefix P, keys which share that prefix are stored at a depth
   * proportional to the length of P. However, the nodes leading up to that
   * node may also contain valid prefixed nodes so we must approach this in
   * two stages:
   *
   * 1) Walk the tree until we hit a depth matching the length of the prefix,
   *    testing nodes individually to see if they match. Issue callbacks if
   *    they do.
   *
   * 2) Once the depths is met (if it exists) we can then enumerate the
   *    entire subtree, issuing callbacks for all nodes in the subtree.
   */

  err = error_NOT_FOUND; /* assume not found until we call the callback */

  depth = 0;

  for (n = t->root; n; n = n->child[dir])
  {
    if (n->item.keylen >= prefixlen &&
        memcmp(n->item.key, prefix, prefixlen) == 0)
    {
      err = cb(&n->item, opaque); /* match */
      if (err)
        return err;
    }

    GET_NEXT_DIR(dir, uprefix, uprefixend);

    if ((size_t) depth == prefixlen * 8)
      break; /* deep enough that all children must match the prefix */
  }

  if (!n)
    return err;

  args.uprefix   = prefix;
  args.prefixlen = prefixlen;
  args.cb        = cb;
  args.opaque    = opaque;

  /* We've processed the node sitting at the position where the prefix is.
   * Now process both of its sub-trees. */

  for (i = 0; i < 2; i++)
  {
    err = dstree__walk_internal_post_node((dstree__node_t *) n->child[i],
                                          depth,
                                          dstree__lookup_prefix_node,
                                          &args);
    if (err)
      return err;
  }

  return error_OK;
}

/* ----------------------------------------------------------------------- */

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

/* in order */
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

error dstree__walk_internal(dstree_t                       *t,
                            dstree__walk_internal_callback *cb,
                            void                           *opaque)
{
  if (t == NULL)
    return error_OK;

  return dstree__node_walk_internal(t->root, 0, cb, opaque);
}

/* ----------------------------------------------------------------------- */

error dstree__breadthwalk_internal(dstree_t                       *t,
                                   dstree__walk_internal_callback *cb,
                                   void                           *opaque)
{
  typedef struct nodedepth
  {
    dstree__node_t *node;
    int             depth;
  }
  nodedepth;

  error     err;
  queue_t  *queue;
  nodedepth nd;

  if (t == NULL)
    return error_OK;

  queue = queue_create(100, sizeof(nodedepth)); /* FIXME: 100 constant */
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
    nodedepth ndc;

    err = queue_dequeue(queue, &nd);
    if (err)
      return err;

    err = cb(nd.node, nd.depth, opaque);
    if (err)
      return err;

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

