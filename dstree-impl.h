/* dstree-impl.h -- associative array implemented as digital search tree */

#ifndef DSTREE_IMPL_H
#define DSTREE_IMPL_H

#include "types.h"

#include "item.h"

#include "dstree.h"

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

  dstree_compare       *compare;
  dstree_destroy_key   *destroy_key;
  dstree_destroy_value *destroy_value;
};

/* ----------------------------------------------------------------------- */

#define IS_LEAF(n) ((n)->child[0] == NULL && (n)->child[1] == NULL)

/* ----------------------------------------------------------------------- */

/* internal tree walk functions which return a pointer to a dstree__node_t */

typedef error (dstree__walk_internal_callback)(dstree__node_t *n,
                                               int             level,
                                               void           *opaque);

error dstree__walk_internal_post(dstree_t                       *t,
                                 dstree__walk_internal_callback *cb,
                                 void                           *opaque);
error dstree__walk_internal(dstree_t                       *t,
                            dstree__walk_internal_callback *cb,
                            void                           *opaque);

error dstree__breadthwalk_internal(dstree_t                       *t,
                                   dstree__walk_internal_callback *cb,
                                   void                           *opaque);

/* ----------------------------------------------------------------------- */

#endif /* DSTREE_IMPL_H */
