/* --------------------------------------------------------------------------
 *    Name: destroy.c
 * Purpose: Hash
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "memento.h"

#include "hash.h"

#include "hash-impl.h"

void hash_destroy(hash_t *h)
{
  int i;

  for (i = 0; i < h->nbins; i++)
    while (h->bins[i])
      hash_remove_node(h, &h->bins[i]);

  free(h->bins);

  free(h);
}
