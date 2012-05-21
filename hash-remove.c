/* --------------------------------------------------------------------------
 *    Name: remove.c
 * Purpose: Hash
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "memento.h"

#include "hash.h"

#include "hash-impl.h"

void hash_remove_node(hash_t *h, hash__node_t **n)
{
  hash__node_t *doomed;

  doomed = *n;

  *n = doomed->next;

  h->destroy_key((void *) doomed->item.key); /* must cast away const */
  h->destroy_value((void *) doomed->item.value); /* must cast away const */

  free(doomed);
}

void hash_remove(hash_t *h, const void *key)
{
  hash__node_t **n;

  n = hash_lookup_node(h, key);
  hash_remove_node(h, n);
}
