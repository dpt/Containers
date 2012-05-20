/* --------------------------------------------------------------------------
 *    Name: lookup-node.c
 * Purpose: Hash
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "hash.h"

#include "hash-impl.h"

node **hash_lookup_node(hash_t *h, const void *key)
{
  int    hash;
  node **n;

  hash = h->hash_fn(key) % h->nbins;
  for (n = &h->bins[hash]; *n != NULL; n = &(*n)->next)
    if (h->compare(key, (*n)->key) == 0)
      break;

  return n;
}
