/* --------------------------------------------------------------------------
 *    Name: impl.h
 * Purpose: Associative array implemented as an ordered array
 * ----------------------------------------------------------------------- */

#ifndef ORDEREDARRAY_IMPL_H
#define ORDEREDARRAY_IMPL_H

#include "base/types.h"

#include "datastruct/item.h"

#include "datastruct/orderedarray.h"

/* ----------------------------------------------------------------------- */

typedef struct orderedarray__node
{
  item_t                      item;
}
orderedarray__node_t;

struct orderedarray
{
  orderedarray__node_t       *array;
  int                         nelems;
  int                         maxelems;

  const void                 *default_value;

  orderedarray_compare       *compare;
  orderedarray_destroy_key   *destroy_key;
  orderedarray_destroy_value *destroy_value;
};

/* ----------------------------------------------------------------------- */

void orderedarray__node_destroy(orderedarray_t       *t,
                                orderedarray__node_t *n);

int orderedarray__lookup_internal(orderedarray_t        *t,
                                  const void            *key,
                                  orderedarray__node_t **n);

/* ----------------------------------------------------------------------- */

/* internal tree walk functions. callback returns a pointer to a
 * orderedarray__node_t, so internal for that reason. */

typedef result_t (orderedarray__walk_internal_callback)(orderedarray__node_t *n,
                                                        void                 *opaque);

result_t orderedarray__walk_internal(orderedarray_t                       *t,
                                     orderedarray__walk_internal_callback *cb,
                                     void                                 *opaque);

/* ----------------------------------------------------------------------- */

#endif /* ORDEREDARRAY_IMPL_H */

