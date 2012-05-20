/* --------------------------------------------------------------------------
 *    Name: remove.c
 * Purpose: Hash
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "memento.h"

#include "hash.h"

#include "hash-impl.h"

void hash_remove_node(hash_t *h, node **n)
{
  node *doomed;

  doomed = *n;

  *n = doomed->next;

  h->destroy_key(doomed->key);
  h->destroy_value(doomed->value);

  free(doomed);
}

void hash_remove(hash_t *h, const void *key)
{
  node **n;

  n = hash_lookup_node(h, key);
  hash_remove_node(h, n);
}
