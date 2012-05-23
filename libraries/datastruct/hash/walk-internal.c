/* --------------------------------------------------------------------------
 *    Name: walk-internal.c
 * Purpose: Hash
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "base/errors.h"

#include "datastruct/hash.h"

#include "impl.h"

error hash__walk_internal(const hash_t                 *hash,
                          hash__walk_internal_callback *cb,
                          void                         *opaque)
{
  error err;
  int   i;

  if (hash == NULL)
    return error_OK;

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

  return error_OK;
}
