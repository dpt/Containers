/* --------------------------------------------------------------------------
 *    Name: walk.c
 * Purpose: Hash
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "memento.h"

#include "errors.h"

#include "hash.h"

#include "hash-impl.h"

error hash_walk(const hash_t *h, hash_walk_callback *cb, void *cbarg)
{
  int i;

  for (i = 0; i < h->nbins; i++)
  {
    hash__node_t *n;
    hash__node_t *next;

    for (n = h->bins[i]; n != NULL; n = next)
    {
      int r;

      next = n->next;

      r = cb(&n->item, cbarg);
      if (r < 0)
        return r;
    }
  }

  return error_OK;
}
