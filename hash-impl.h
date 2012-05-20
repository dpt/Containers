/* --------------------------------------------------------------------------
 *    Name: impl.c
 * Purpose: Hash
 * ----------------------------------------------------------------------- */

#ifndef HASH_IMPL_H
#define HASH_IMPL_H

#include "hash.h"

typedef struct node
{
  struct node         *next;
  void                *key;
  void                *value;
}
node;

struct hash_t
{
  hash_fn            *hash_fn;
  hash_compare       *compare;
  hash_destroy_key   *destroy_key;
  hash_destroy_value *destroy_value;
  int                 nbins;
  node              **bins;
};

node **hash_lookup_node(hash_t *h, const void *key);
void hash_remove_node(hash_t *h, node **n);

#endif /* HASH_IMPL_H */
