/* --------------------------------------------------------------------------
 *    Name: impl.c
 * Purpose: Hash
 * ----------------------------------------------------------------------- */

#ifndef HASH_IMPL_H
#define HASH_IMPL_H

#include <stdlib.h>

#include "item.h"

#include "hash.h"

typedef struct node // rename hash__node
{
  struct node *next;
  item_t       item;
}
node;

struct hash_t
{
  node              **bins;
  int                 nbins;

  int                 count;

  const void         *default_value;

  hash_fn            *hash_fn;
  hash_compare       *compare;
  hash_destroy_key   *destroy_key;
  hash_destroy_value *destroy_value;
};

node **hash_lookup_node(hash_t *h, const void *key);
void hash_remove_node(hash_t *h, node **n);

#endif /* HASH_IMPL_H */
