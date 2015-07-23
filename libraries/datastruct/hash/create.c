/* --------------------------------------------------------------------------
 *    Name: create.c
 * Purpose: Associative array implemented as a hash
 * ----------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>

#include "base/memento/memento.h"

#include "base/result.h"
#include "utils/utils.h"
#include "utils/primes.h"

#include "datastruct/hash.h"

#include "impl.h"

/* ----------------------------------------------------------------------- */

result_t hash_create(const void         *default_value,
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
    return result_OOM;

  nbins = prime_nearest(nbins);

  bins = calloc(nbins, sizeof(*h->bins));
  if (bins == NULL)
  {
    free(h);
    return result_OOM;
  }

  h->bins          = bins;
  h->nbins         = nbins;

  h->count         = 0;

  h->default_value = default_value;

  h->hash_fn       = fn;
  h->compare       = compare;
  h->destroy_key   = destroy_key;
  h->destroy_value = destroy_value;

  *ph = h;

  return result_OK;
}
