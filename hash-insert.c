/* --------------------------------------------------------------------------
 *    Name: insert.c
 * Purpose: Hash
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "memento.h"

#include "errors.h"

#include "hash.h"

#include "hash-impl.h"

error hash_insert(hash_t *h, void *key, void *value)
{
  node **n;
  int    hash;

  n = hash_lookup_node(h, key);
  if (*n)
  {
    /* already exists: update the value */

    h->destroy_value((*n)->value);

    (*n)->value = value;

    h->destroy_key(key);
  }
  else
  {
    node *m;

    /* not found: create new node */

    m = malloc(sizeof(*m));
    if (m == NULL)
      return error_OOM;

    hash = h->hash_fn(key) % h->nbins;

    m->next  = NULL;
    m->key   = key;
    m->value = value;

    *n = m;
  }

  return error_OK;
}
