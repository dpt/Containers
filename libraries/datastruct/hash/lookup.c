/* --------------------------------------------------------------------------
 *    Name: lookup.c
 * Purpose: Hash
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "datastruct/hash.h"

#include "impl.h"

const void *hash_lookup(hash_t *h, const void *key)
{
  hash__node_t **n;

  n = hash_lookup_node(h, key);

  return (*n != NULL) ? (*n)->item.value : h->default_value;
}
