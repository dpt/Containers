/* --------------------------------------------------------------------------
 *    Name: walk.c
 * Purpose: Associative array implemented as a hash
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "base/result.h"

#include "datastruct/hash.h"

#include "impl.h"

result_t hash_walk(const hash_t *h, hash_walk_callback *cb, void *cbarg)
{
  int i;

  for (i = 0; i < h->nbins; i++)
  {
    hash__node_t *n;
    hash__node_t *next;

    for (n = h->bins[i]; n != NULL; n = next)
    {
      result_t r;

      next = n->next;

      r = cb(&n->item, cbarg);
      if (r)
        return r;
    }
  }

  return result_OK;
}

