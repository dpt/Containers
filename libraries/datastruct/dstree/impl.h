/* --------------------------------------------------------------------------
 *    Name: impl.h
 * Purpose: Associative array implemented as a digital search tree
 * ----------------------------------------------------------------------- */

#ifndef DSTREE_IMPL_H
#define DSTREE_IMPL_H

#include <stddef.h>

#include "base/types.h"

#include "datastruct/item.h"

#include "datastruct/dstree.h"

/* ----------------------------------------------------------------------- */

typedef struct dstree__node
{
  /* using an array here rather than separate left,right elements makes some
   * operations more convenient */
  struct dstree__node  *child[2]; /* left, right children */
  item_t                item;
}
dstree__node_t;

struct dstree
{
  dstree__node_t       *root;

  int                   count;

  const void           *default_value;

  dstree_destroy_key   *destroy_key;
  dstree_destroy_value *destroy_value;
};

/* ----------------------------------------------------------------------- */

#define IS_LEAF(n) ((n)->child[0] == NULL && (n)->child[1] == NULL)

/* ----------------------------------------------------------------------- */

dstree__node_t *dstree__node_create(dstree_t   *t,
                                    const void *key,
                                    const void *value,
                                    size_t      keylen);

void dstree__node_clear(dstree_t *t, dstree__node_t *n);

void dstree__node_destroy(dstree_t *t, dstree__node_t *n);

/* ----------------------------------------------------------------------- */

/* internal tree walk functions which return a pointer to a dstree__node_t */

typedef error (dstree__walk_internal_callback)(dstree__node_t *n,
                                               int             level,
                                               void           *opaque);

error dstree__walk_internal_post(dstree_t                       *t,
                                 dstree__walk_internal_callback *cb,
                                 void                           *opaque);
error dstree__walk_internal_post_node(dstree__node_t                 *root,
                                      int                             level,
                                      dstree__walk_internal_callback *cb,
                                      void                           *opaque);
error dstree__walk_internal(dstree_t                       *t,
                            dstree__walk_internal_callback *cb,
                            void                           *opaque);

/* ----------------------------------------------------------------------- */

error dstree__breadthwalk_internal(dstree_t                       *t,
                                   dstree__walk_internal_callback *cb,
                                   void                           *opaque);

/* ----------------------------------------------------------------------- */

/* Extract the next binary direction from the key.
 * Within a byte the MSB is extracted first.
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

#endif /* DSTREE_IMPL_H */

