/* --------------------------------------------------------------------------
 *    Name: walk-internal.c
 * Purpose: Associative array implemented as a hash
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "base/result.h"

#include "datastruct/hash.h"

#include "impl.h"

result_t hash__walk_internal(const hash_t                 *hash,
                             hash__walk_internal_callback *cb,
                             void                         *opaque)
{
  result_t err;
  int      i;

  if (hash == NULL)
    return result_OK;

  for (i = 0; i < hash->nbins; i++)
  {
    int           j;
    hash__node_t *n;
    hash__node_t *next;

    j = 0;
    for (n = hash->bins[i]; n != NULL; n = next)
    {
      next = n->next;

      err = cb(n, i, j, opaque);
      if (err)
        return err;

      j++;
    }
  }

  return result_OK;
}
