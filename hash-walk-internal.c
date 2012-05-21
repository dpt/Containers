/* --------------------------------------------------------------------------
 *    Name: walk-internal.c
 * Purpose: Hash
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "memento.h"

#include "errors.h"

#include "hash.h"

#include "hash-impl.h"

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
    hash__node_t *n;
    hash__node_t *next;

    for (n = hash->bins[i]; n != NULL; n = next)
    {
      next = n->next;

      err = cb(n, opaque);
      if (err)
        return err;
    }
  }

  return error_OK;
}
