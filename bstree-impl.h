/* bstree-impl.h -- associative array implemented as binary search tree */

#ifndef BSTREE_IMPL_H
#define BSTREE_IMPL_H

#include "item.h"

#include "bstree.h"

/* ----------------------------------------------------------------------- */

typedef struct bstree__node
{
  /* using an array here rather than separate left,right elements makes some
   * operations more convenient */
  struct bstree__node  *child[2]; /* left, right children */
  item_t                item;
}
bstree__node_t;

struct bstree
{
  bstree__node_t       *root;

  int                   count;

  const void           *default_value;

  bstree_compare       *compare;
  bstree_destroy_key   *destroy_key;
  bstree_destroy_value *destroy_value;
};

/* ----------------------------------------------------------------------- */

/* internal tree walk functions which return a pointer to a bstree__node_t */

typedef error (bstree__walk_internal_callback)(bstree__node_t *n,
                                               int             level,
                                               void           *opaque);

error bstree__walk_internal_post(bstree_t                       *t,
                                 bstree__walk_internal_callback *cb,
                                 void                           *opaque);
error bstree__walk_internal(bstree_t                       *t,
                            bstree__walk_internal_callback *cb,
                            void                           *opaque);

/* ----------------------------------------------------------------------- */

#endif /* BSTREE_IMPL_H */
