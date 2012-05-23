/* impl.h -- associative array implemented as trie */

#ifndef TRIE_IMPL_H
#define TRIE_IMPL_H

#include "base/types.h"

#include "datastruct/item.h"

#include "datastruct/trie.h"

/* ----------------------------------------------------------------------- */

typedef struct trie__node
{
  /* using an array here rather than separate left,right elements makes some
   * operations more convenient */
  struct trie__node  *child[2]; /* left, right children */
  item_t              item;
}
trie__node_t;

struct trie
{
  trie__node_t       *root;

  int                 count;

  const void         *default_value;

  trie_destroy_key   *destroy_key;
  trie_destroy_value *destroy_value;
};

/* ----------------------------------------------------------------------- */

#define IS_LEAF(n) ((n)->child[0] == NULL && (n)->child[1] == NULL)

/* ----------------------------------------------------------------------- */

typedef unsigned int trie_walk_flags;

#define trie_WALK_ORDER_MASK (3u << 0)
#define trie_WALK_IN_ORDER   (0u << 0)
#define trie_WALK_PRE_ORDER  (1u << 0)
#define trie_WALK_POST_ORDER (2u << 0)

#define trie_WALK_LEAVES     (1u << 2)
#define trie_WALK_BRANCHES   (1u << 3)
#define trie_WALK_ALL        (trie_WALK_LEAVES | trie_WALK_BRANCHES)

/* internal tree walk functions which return a pointer to a trie__node_t */

typedef error (trie__walk_internal_callback)(trie__node_t *n,
                                             int           level,
                                             void         *opaque);

error trie__walk_internal(trie_t                       *t,
                          trie_walk_flags               flags,
                          trie__walk_internal_callback *cb,
                          void                         *opaque);

/* ----------------------------------------------------------------------- */

error trie__breadthwalk_internal(trie_t                       *t,
                                 trie_walk_flags               flags,
                                 trie__walk_internal_callback *cb,
                                 void                         *opaque);

/* ----------------------------------------------------------------------- */

#endif /* TRIE_IMPL_H */
