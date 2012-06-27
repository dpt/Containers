/* --------------------------------------------------------------------------
 *    Name: impl.h
 * Purpose: Associative array implemented as a PATRICIA tree
 * ----------------------------------------------------------------------- */

#ifndef PATRICIA_IMPL_H
#define PATRICIA_IMPL_H

#include <stddef.h>

#include "base/types.h"

#include "datastruct/item.h"

#include "datastruct/patricia.h"

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

patricia__node_t *patricia__node_create(patricia_t *t,
                                        const void *key,
                                        size_t      keylen,
                                        const void *value);

void patricia__node_clear(patricia_t *t, patricia__node_t *n);

void patricia__node_destroy(patricia_t *t, patricia__node_t *n);

const patricia__node_t *patricia__lookup(const patricia__node_t *n,
                                         const void             *key,
                                         size_t                  keylen);

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

/* Gets a byte or returns zero if it's out of range.
 * Copes with negative indices. */
// Have chosen to use size_t for 'unsigned ptrdiff_t'.
#define GET_BYTE(KEY, KEYEND, INDEX) \
  (((size_t) (INDEX) < (size_t) ((KEYEND) - (KEY))) ? (KEY)[INDEX] : 0)

/* Extract the specified indexed binary direction from the key. */
#define GET_DIR(KEY, KEYEND, INDEX) \
  (KEY ? (GET_BYTE(KEY, KEYEND, INDEX >> 3) & (1 << (7 - ((INDEX) & 7)))) != 0 : 0)

/* ----------------------------------------------------------------------- */

#endif /* PATRICIA_IMPL_H */

