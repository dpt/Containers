/* --------------------------------------------------------------------------
 *    Name: impl.h
 * Purpose: Linked list
 * ----------------------------------------------------------------------- */

#ifndef LINKEDLIST_IMPL_H
#define LINKEDLIST_IMPL_H

#include "base/types.h"

#include "datastruct/item.h"

#include "datastruct/linkedlist.h"

/* ----------------------------------------------------------------------- */

typedef struct linkedlist__node
{
  struct linkedlist__node  *next;
  item_t                    item;
}
linkedlist__node_t;

struct linkedlist
{
  linkedlist__node_t       *anchor;

  int                       count;

  const void               *default_value;

  linkedlist_compare       *compare;
  linkedlist_destroy_key   *destroy_key;
  linkedlist_destroy_value *destroy_value;
};

/* ----------------------------------------------------------------------- */

linkedlist__node_t *linkedlist__node_create(linkedlist_t *t,
                                            const void   *key,
                                            size_t        keylen,
                                            const void   *value);

void linkedlist__node_destroy(linkedlist_t       *t,
                              linkedlist__node_t *n);

/* ----------------------------------------------------------------------- */

/* internal tree walk functions. callback returns a pointer to a
 * linkedlist__node_t, so internal for that reason. */

typedef error (linkedlist__walk_internal_callback)(linkedlist__node_t *n,
                                                   void               *opaque);

error linkedlist__walk_internal(linkedlist_t                       *t,
                                linkedlist__walk_internal_callback *cb,
                                void                               *opaque);

/* ----------------------------------------------------------------------- */

#endif /* LINKEDLIST_IMPL_H */

