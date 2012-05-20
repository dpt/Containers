/* bstree.c -- associative array implemented as binary search tree */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memento.h"

#include "errors.h"
#include "utils.h"
#include "string.h"
#include "string-kv.h"

#include "bstree.h"

#include "bstree-impl.h"

/* ----------------------------------------------------------------------- */

static bstree__node_t *bstree__node_create(bstree_t   *t,
                                           const void *key,
                                           size_t      keylen,
                                           const void *value)
{
  bstree__node_t *n;

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

static void bstree__node_destroy_item(bstree_t *t, bstree__node_t *n)
{
  if (t->destroy_key)
    t->destroy_key((void *) n->item.key); /* must cast away const */
  if (t->destroy_value)
    t->destroy_value((void *) n->item.value);
}

static void bstree__node_destroy(bstree_t *t, bstree__node_t *n)
{
  bstree__node_destroy_item(t, n);

  free(n);

  t->count--;
}

/* ----------------------------------------------------------------------- */

error bstree_create(const void            *default_value,
                    bstree_compare        *compare,
                    bstree_destroy_key    *destroy_key,
                    bstree_destroy_value  *destroy_value,
                    bstree_t             **pt)
{
  bstree_t *t;

  *pt = NULL;

  t = malloc(sizeof(*t));
  if (t == NULL)
    return error_OOM;

  t->root          = NULL;
  t->default_value = default_value;
  t->compare       = compare;
  t->destroy_key   = destroy_key;
  t->destroy_value = destroy_value;

  t->count         = 0;

  *pt = t;

  return error_OK;
}

static error bstree__destroy_node(bstree__node_t *n, int level, void *opaque)
{
  NOT_USED(level);

  bstree__node_destroy(opaque, n);

  return error_OK;
}

void bstree_destroy(bstree_t *t)
{
  (void) bstree__walk_internal_post(t, bstree__destroy_node, t);

  free(t);
}

/* ----------------------------------------------------------------------- */

static INLINE const void *bstree__lookup_node(const bstree__node_t *n,
                                              const void           *key,
                                              const void           *default_value,
                                              bstree_compare       *compare)
{
  while (n)
  {
    int d;

    d = compare(key, n->item.key);
    if (d == 0)
      return n->item.value; /* found */

    n = n->child[d < 0 ? 0 : 1];
  }

  return default_value; /* not found */
}

const void *bstree_lookup(bstree_t *t, const void *key)
{
  return bstree__lookup_node(t->root, key, t->default_value, t->compare);
}

/* ----------------------------------------------------------------------- */

static INLINE bstree__node_t **bstree__insert_node(bstree__node_t **pn,
                                                   const void      *key,
                                                   bstree_compare  *compare)
{
  bstree__node_t *n;

  while ((n = *pn) != NULL)
  {
    int d;

    d = compare(key, n->item.key);
    if (d == 0)
      return NULL; /* found match */

    pn = &n->child[d < 0 ? 0 : 1];
  }

  return pn; /* found insertion point */
}

error bstree_insert(bstree_t *t, const void *key, size_t keylen, const void *value)
{
  bstree__node_t **pn;

  pn = bstree__insert_node(&t->root, key, t->compare);
  if (pn == NULL)
    return error_EXISTS;

  *pn = bstree__node_create(t, key, keylen, value);
  if (*pn == NULL)
    return error_OOM;

  return error_OK;
}

/* ----------------------------------------------------------------------- */

void bstree_remove(bstree_t *t, const void *key)
{
  bstree__node_t **pn;
  bstree__node_t  *n;

  pn = &t->root;
  n  = *pn;
  while (n)
  {
    int d;

    d = t->compare(key, n->item.key);
    if (d == 0)
      break;

    pn = &n->child[d < 0 ? 0 : 1];
    n  = *pn;
  }

  if (n == NULL)
    return; /* not found */

  bstree__node_destroy_item(t, n);

  /* case 1: node has no children */
  if (n->child[0] == NULL && n->child[1] == NULL)
  {
    /* set parent to NULL */
    *pn = NULL;
  }
  /* case 2: node has just one child */
  else if (n->child[0] == NULL || n->child[1] == NULL)
  {
    /* point parent to grandchild */
    *pn = n->child[0] ? n->child[0] : n->child[1];
  }
  /* case 3: node has both children */
  else
  {
    bstree__node_t **pmin;
    bstree__node_t  *min;

    /* find minimum node in right subtree */
    pmin = &n->child[1];
    min  = *pmin;
    while (min->child[0])
    {
      pmin = &min->child[0];
      min  = *pmin;
    }

    /* take minimum node's item to replace existing item */
    n->item = min->item;

    /* minimum is now a duplicate: remove it */
    *pmin = min->child[1]; /* right child, or NULL */

    n = min;
  }

  free(n);

  t->count--;
}

/* ----------------------------------------------------------------------- */

int bstree_count(bstree_t *t)
{
  return t->count;
}

/* ----------------------------------------------------------------------- */

typedef struct bstree_lookup_prefix_args
{
  const unsigned char   *uprefix;
  size_t                 prefixlen;
  bstree_found_callback *cb;
  void                  *opaque;
  int                    found;
}
bstree_lookup_prefix_args_t;

static error bstree__lookup_prefix(bstree__node_t *n,
                                   int             level,
                                   void           *opaque)
{
  bstree_lookup_prefix_args_t *args = opaque;
  size_t                       len;

  NOT_USED(level);

  len = args->prefixlen;

  if (n->item.keylen >= len && memcmp(n->item.key, args->uprefix, len) == 0)
  {
    args->found = 1;
    return args->cb(&n->item, args->opaque);
  }

  return error_OK;
}

/* Walk the entire tree looking for matches.
 * Is there a more cunning way than this? */
error bstree_lookup_prefix(const bstree_t        *t,
                           const void            *prefix,
                           size_t                 prefixlen,
                           bstree_found_callback *cb,
                           void                  *opaque)
{
  error                       err;
  bstree_lookup_prefix_args_t args;

  args.uprefix   = prefix;
  args.prefixlen = prefixlen;
  args.cb        = cb;
  args.opaque    = opaque;
  args.found     = 0;

  err = bstree__walk_internal((bstree_t *) t, /* must cast away constness */
                              bstree__lookup_prefix,
                              &args);
  if (err)
    return err;

  return args.found ? error_OK : error_NOT_FOUND;
}

/* ----------------------------------------------------------------------- */

static error bstree__node_walk_internal_post(bstree__node_t                 *n,
                                             int                             level,
                                             bstree__walk_internal_callback *cb,
                                             void                           *opaque)
{
  error err;

  if (n == NULL)
    return error_OK;

  err = bstree__node_walk_internal_post(n->child[0], level + 1, cb, opaque);
  if (!err)
    err = bstree__node_walk_internal_post(n->child[1], level + 1, cb, opaque);
  if (!err)
    err = cb(n, level, opaque);

  return err;
}

error bstree__walk_internal_post(bstree_t                       *t,
                                 bstree__walk_internal_callback *cb,
                                 void                           *opaque)
{
  if (t == NULL)
    return error_OK;

  return bstree__node_walk_internal_post(t->root, 0, cb, opaque);
}

/* in-order */
static error bstree__node_walk_internal(bstree__node_t                 *n,
                                        int                             level,
                                        bstree__walk_internal_callback *cb,
                                        void                           *opaque)
{
  error err;

  if (n == NULL)
    return error_OK;

  err = bstree__node_walk_internal(n->child[0], level + 1, cb, opaque);
  if (!err)
    err = cb(n, level, opaque);
  if (!err)
    err = bstree__node_walk_internal(n->child[1], level + 1, cb, opaque);

  return err;
}

// in-order
error bstree__walk_internal(bstree_t                       *t,
                            bstree__walk_internal_callback *cb,
                            void                           *opaque)
{
  if (t == NULL)
    return error_OK;

  return bstree__node_walk_internal(t->root, 0, cb, opaque);
}

/* ----------------------------------------------------------------------- */
