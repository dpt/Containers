/* --------------------------------------------------------------------------
 *    Name: insert.c
 * Purpose: Associative array implemented as a hash
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "base/errors.h"

#include "datastruct/hash.h"

#include "impl.h"

error hash_insert(hash_t     *h,
                  const void *key,
                  size_t      keylen,
                  const void *value)
{
  hash__node_t **n;

  n = hash_lookup_node(h, key); /* must cast away const */
  if (*n)
  {
    /* already exists: update the value */

    h->destroy_value((void *) (*n)->item.value); /* must cast away const */

    (*n)->item.value = value;

    h->destroy_key((void *) key); /* must cast away const */
  }
  else
  {
    hash__node_t *m;

    /* not found: create new node */

    m = malloc(sizeof(*m));
    if (m == NULL)
      return error_OOM;

    m->next        = NULL;
    m->item.key    = key;
    m->item.keylen = keylen;
    m->item.value  = value;

    h->count++;

    *n = m;
  }

  return error_OK;
}
