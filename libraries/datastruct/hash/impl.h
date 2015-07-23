/* --------------------------------------------------------------------------
 *    Name: impl.c
 * Purpose: Associative array implemented as a hash
 * ----------------------------------------------------------------------- */

#ifndef HASH_IMPL_H
#define HASH_IMPL_H

#include <stdlib.h>

#include "datastruct/item.h"

#include "datastruct/hash.h"

/* ----------------------------------------------------------------------- */

typedef struct hash__node
{
  struct hash__node *next;
  item_t             item;
}
hash__node_t;

struct hash
{
  hash__node_t      **bins;
  unsigned int        nbins;

  int                 count;

  const void         *default_value;

  hash_fn            *hash_fn;
  hash_compare       *compare;
  hash_destroy_key   *destroy_key;
  hash_destroy_value *destroy_value;
};

/* ----------------------------------------------------------------------- */

/* internal hash walk functions which return a pointer to a hash__node_t */

typedef result_t (hash__walk_internal_callback)(hash__node_t *node,
                                                int           bin,
                                                int           index,
                                                void         *opaque);

result_t hash__walk_internal(const hash_t                 *hash,
                             hash__walk_internal_callback *cb,
                             void                         *opaque);

/* ----------------------------------------------------------------------- */

hash__node_t **hash_lookup_node(hash_t *h, const void *key);
void hash_remove_node(hash_t *h, hash__node_t **n);

/* ----------------------------------------------------------------------- */

#endif /* HASH_IMPL_H */
