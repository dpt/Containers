/* dstree.c -- associative array implemented as digital search tree */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memento.h"

#include "types.h"
#include "string.h"
#include "string-kv.h"

#include "dstree.h"

#include "dstree-impl.h"

/* ----------------------------------------------------------------------- */

static dstree__node_t *dstree__node_create(dstree_t   *t,
                                           const void *key,
                                           const void *value)
{
  dstree__node_t *n;

  n = malloc(sizeof(*n));
  if (n == NULL)
    return NULL;

  n->child[0]   = NULL;
  n->child[1]   = NULL;
  n->item.key   = key;
  n->item.value = value;

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

/* NULL passed into the bit/compare/destroy callbacks signifies that these
 * internal routines should be used. They are setup to handle malloc'd strings.
 */

error dstree_create(const void            *default_value,
                    dstree_compare        *compare,
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

  /* if NULL is specified for callbacks then default to handlers suitable for
   * a malloc'd string */

  t->compare       = compare       ? compare       : stringkv_compare;
  t->destroy_key   = destroy_key   ? destroy_key   : stringkv_destroy;
  t->destroy_value = destroy_value ? destroy_value : stringkv_destroy;

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
  (void) dstree__walk_internal(t, dstree__destroy_node, t);

  free(t);
}

/* ----------------------------------------------------------------------- */

/* Extract the next binary direction from the key.
 * Within a byte the MSB is extacted first.
 */
#define GET_NEXT_DIR(DIR)                \
do                                       \
{                                        \
  if ((depth++ & 7) == 0)                \
    c = (ukey == ukeyend) ? 0 : *ukey++; \
                                         \
  DIR = (c & 0x80) != 0;                 \
  c <<= 1;                               \
}                                        \
while (0)

/* ----------------------------------------------------------------------- */

const void *dstree_lookup(dstree_t *t, const void *key, size_t keylen)
{
  const unsigned char  *ukey    = key;
  const unsigned char  *ukeyend = ukey + keylen;
  int                   depth;
  const dstree__node_t *n;
  int                   dir;
  unsigned char         c;
  
  depth = 0;
  
  for (n = t->root; n; n = n->child[dir])
  {
    if (t->compare(key, n->item.key) == 0)
      return n->item.value; /* found */
    
    GET_NEXT_DIR(dir);
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
  unsigned char        c;

  depth = 0;
  
  for (pn = &t->root; (n = *pn); pn = &n->child[dir])
  {
    if (t->compare(key, n->item.key) == 0)
      return error_EXISTS;
    
    GET_NEXT_DIR(dir);
  }

  *pn = dstree__node_create(t, key, value);
  if (*pn == NULL)
    return error_OOM;

  return error_OK;
}

/* ----------------------------------------------------------------------- */

// A = 0000
// B = 0001
// C = 0011
// D = 0111
// E = 1111
//
// Inserting as ABCDE we get:
//
//     A      A = xxxx
//    / \     B = 0xxx
//   B   E    C = 00xx
//  / \       D = 01xx
// C   D      E = 1xxx
//
// Inserting as EDCBA we get:
//
//         E    E = xxxx
//        /     D = 0xxx
//       D      C = 00xx
//      /       B = 000x
//     C        A = 0000
//    /
//   B
//  /
// A
//
// Inserting as CAEDB we get:
//
//     C      C = xxxx
//    / \     A = 0xxx
//   A   E    E = 1xxx
//  / \       D = 01xx
// B   D      B = 00xx
//
//
//     C      C = xxxx    this level holds any value
//    / \     A = 0xxx
//   A   E    E = 1xxx    this level holds prefixes 0 or 1
//  / \       D = 01xx
// B   D      B = 00xx    this level holds prefixes 00 or 01 or 10 or 11
//
// So if there is a single subtree it can always take the place of its parent,
// since the initial portion of the key up to that level is the same. << WRONG
//
// If I remove a node with two children, how do I reorganise the tree?
// Any descendant can take the place of its ancestor so we can find any leaf
// node in its subtree and stash it in place of its parent.

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
  
  for (pn = &t->root; (n = *pn); pn = &n->child[dir])
  {
    if (t->compare(n->item.key, key) == 0)
      break; /* found */
    
    GET_NEXT_DIR(dir);
  }
  
  if (n == NULL)
    return; /* doesn't exist */

  /* pn now points to pointer to the doomed node */

  /* if we find a leaf node, just zap the parent pointer */
  if (IS_LEAF(n))
  {
    *pn = NULL;
    dstree__node_destroy(t, n);
    return;
  }

  /* otherwise we look for a leaf node that we can swap into the place of
   * the parent node */

  m = n;

  // this comment is out of date now
  /* dstree__remove above doesn't yield us the depth of the tree where the node
   * was located. this perhaps doesn't matter as all we need is /any/ leaf node
   * downward of the to-be-removed node. so we just set depth to zero here and
   * hope that the binary values GET_NEXT_DIR() returns cut us a random path through
   the tree. */
  ukey = key; // reset
  depth = 0;

  do
  {
    /* if there's no right child, go left.
     * if there's no left child, go right.
     * if both are present, fetch a value from GET_NEXT_DIR() and choose.
     * this means that we only call it when necessary. */

    // why does this look wrong to me now - if right is absent that doesn't guarantee that left is present on the first iteration does it?
    // i suppose it does since we're guaranteed a non-leaf node here - could add an assert to that effect
    
    if (m->child[1] == NULL)
      pm = &m->child[0];
    else if (m->child[0] == NULL)
      pm = &m->child[1];
    else
    {
      GET_NEXT_DIR(dir);     // this does its own depth++ which conflicts with the local technique of doing it later...
      pm = &m->child[dir];
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
  unsigned char        c;
  int                  dir;
  dstree__node_t      *root;
  dstree__node_t      *side;

  depth = 0;

  for (pn = &t->root; (n = *pn); pn = &n->child[dir])
  {
    if (t->compare(n->item.key, key) == 0)
      break; /* found */
    
    GET_NEXT_DIR(dir);
  }

  if (n == NULL)
    return; /* not found */

  /* pick a random child. if that child's absent then pick the other */
  GET_NEXT_DIR(dir);
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

    GET_NEXT_DIR(newdir);
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

/* post-order (which allows for deletions) */
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
    err = dstree__node_walk_internal(n->child[1], level + 1, cb, opaque);
  if (!err)
    err = cb(n, level, opaque);

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
