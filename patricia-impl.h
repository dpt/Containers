/* patricia-impl.h -- associative array implemented as patricia trie */

#ifndef PATRICIA_IMPL_H
#define PATRICIA_IMPL_H

#include "types.h"

#include "item.h"

#include "patricia.h"

/* ----------------------------------------------------------------------- */

typedef struct patricia__node
{
  /* using an array here rather than separate left,right elements makes some
   * operations more convenient */
  struct patricia__node    *child[2];  /* left, right children */
  int                       bit;       /* critical bit */
  item_t                    item;
}
patricia__node_t;

struct patricia
{
  patricia__node_t         *root;

  int                       count;

  const void               *default_value;

  patricia_destroy_key     *destroy_key;
  patricia_destroy_value   *destroy_value;
};

/* ----------------------------------------------------------------------- */

typedef unsigned int patricia_walk_flags;

#define patricia_WALK_ORDER_MASK (3u << 0)
#define patricia_WALK_IN_ORDER   (0u << 0)
#define patricia_WALK_PRE_ORDER  (1u << 0)
#define patricia_WALK_POST_ORDER (2u << 0)

#define patricia_WALK_LEAVES     (1u << 2)
#define patricia_WALK_BRANCHES   (1u << 3)
#define patricia_WALK_ALL        (patricia_WALK_LEAVES | patricia_WALK_BRANCHES)

/* internal tree walk functions. callback returns a pointer to a
 * patricia__node_t, so internal for that reason. */

typedef error (patricia__walk_internal_callback)(patricia__node_t *n,
                                                 int               level,
                                                 void             *opaque);

error patricia__walk_internal(patricia_t                       *t,
                              patricia_walk_flags               flags,
                              patricia__walk_internal_callback *cb,
                              void                             *opaque);

/* ----------------------------------------------------------------------- */

#endif /* PATRICIA_IMPL_H */
