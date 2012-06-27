/* --------------------------------------------------------------------------
 *    Name: walk.c
 * Purpose: Associative array implemented as a hash
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "base/errors.h"

#include "datastruct/hash.h"

#include "impl.h"

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

