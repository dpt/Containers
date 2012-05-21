/* --------------------------------------------------------------------------
 *    Name: impl.c
 * Purpose: Hash
 * ----------------------------------------------------------------------- */

#ifndef HASH_IMPL_H
#define HASH_IMPL_H

#include <stdlib.h>

#include "item.h"

#include "hash.h"

typedef struct hash__node
{
  struct hash__node *next;
  item_t             item;
}
hash__node_t;

struct hash
{
  hash__node_t      **bins;
  int                 nbins;

  int                 count;

  const void         *default_value;

  hash_fn            *hash_fn;
  hash_compare       *compare;
  hash_destroy_key   *destroy_key;
  hash_destroy_value *destroy_value;
};

hash__node_t **hash_lookup_node(hash_t *h, const void *key);
void hash_remove_node(hash_t *h, hash__node_t **n);

#endif /* HASH_IMPL_H */
