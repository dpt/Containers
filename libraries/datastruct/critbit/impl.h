/* impl.h -- associative array implemented as critbit tree */

#ifndef CRITBIT_IMPL_H
#define CRITBIT_IMPL_H

#include "base/types.h"

#include "datastruct/item.h"

#include "datastruct/critbit.h"

/* ----------------------------------------------------------------------- */

/* an external node */
typedef struct critbit__extnode
{
  item_t                   item;
}
critbit__extnode_t;

/* an internal node */
typedef struct critbit__node
{
  /* using an array here rather than separate left,right elements makes some
   * operations more convenient */
  struct critbit__node    *child[2];  /* left, right children */
  int                      byte;      /* byte offset of critical bit */
  unsigned char            otherbits; /* inverted mask of critical bit */
}
critbit__node_t;

struct critbit
{
  critbit__node_t         *root;

  int                      intcount; /* count of internal nodes */
  int                      extcount; /* count of external nodes */

  const void              *default_value;

  critbit_destroy_key     *destroy_key;
  critbit_destroy_value   *destroy_value;
};

/* ----------------------------------------------------------------------- */

/* These have a reversed sense compared to the original paper. */
#define IS_INTERNAL(p) (((intptr_t) (p) & 1) == 0)
#define IS_EXTERNAL(p) (!IS_INTERNAL(p))

/* Take a node pointer and make it into an extnode. */
#define FROM_STORE(p) (critbit__extnode_t *) ((intptr_t) (p) - 1)
/* Take an extnode and make it into a node pointer. */
#define TO_STORE(p) (critbit__node_t *) ((intptr_t) (p) + 1)

/* ----------------------------------------------------------------------- */

typedef unsigned int critbit_walk_flags;

#define critbit_WALK_ORDER_MASK (3u << 0)
#define critbit_WALK_IN_ORDER   (0u << 0)
#define critbit_WALK_PRE_ORDER  (1u << 0)
#define critbit_WALK_POST_ORDER (2u << 0)

#define critbit_WALK_LEAVES     (1u << 2)
#define critbit_WALK_BRANCHES   (1u << 3)
#define critbit_WALK_ALL        (critbit_WALK_LEAVES | critbit_WALK_BRANCHES)

/* internal tree walk functions. callback returns a pointer to a
 * critbit__node_t, so internal for that reason. */

typedef error (critbit__walk_internal_callback)(critbit__node_t *n,
                                                int              level,
                                                void            *opaque);

error critbit__walk_internal(critbit_t                       *t,
                             critbit_walk_flags               flags,
                             critbit__walk_internal_callback *cb,
                             void                            *opaque);

/* ----------------------------------------------------------------------- */

#endif /* CRITBIT_IMPL_H */
