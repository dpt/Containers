/* --------------------------------------------------------------------------
 *    Name: remove.c
 * Purpose: Hash
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "datastruct/hash.h"

#include "impl.h"

void hash_remove_node(hash_t *h, hash__node_t **n)
{
  hash__node_t *doomed;

  doomed = *n;

  *n = doomed->next;

  h->destroy_key((void *) doomed->item.key); /* must cast away const */
  h->destroy_value((void *) doomed->item.value); /* must cast away const */

  free(doomed);

  h->count--;
}

void hash_remove(hash_t *h, const void *key)
{
  hash__node_t **n;

  n = hash_lookup_node(h, key);
  hash_remove_node(h, n);
}
