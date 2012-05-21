/* --------------------------------------------------------------------------
 *    Name: create.c
 * Purpose: Hash
 * ----------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>

#include "memento.h"

#include "errors.h"
#include "utils.h"

#include "primes.h"

#include "hash.h"

#include "hash-impl.h"

/* ----------------------------------------------------------------------- */

error hash_create(const void         *default_value,
                  int                 nbins,
                  hash_fn            *fn,
                  hash_compare       *compare,
                  hash_destroy_key   *destroy_key,
                  hash_destroy_value *destroy_value,
                  hash_t            **ph)
{
  hash_t        *h;
  hash__node_t **bins;

  h = malloc(sizeof(*h));
  if (h == NULL)
    return error_OOM;

  nbins = prime_nearest(nbins);

  bins = calloc(nbins, sizeof(*h->bins));
  if (bins == NULL)
    return error_OOM;

  h->bins          = bins;
  h->nbins         = nbins;

  h->count         = 0;

  h->default_value = default_value;

  h->hash_fn       = fn;
  h->compare       = compare;
  h->destroy_key   = destroy_key;
  h->destroy_value = destroy_value;

  *ph = h;

  return error_OK;
}
