/* --------------------------------------------------------------------------
 *    Name: walk.c
 * Purpose: Hash
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "memento.h"

//#include "appengine/types.h"
#include "errors.h"
//#include "appengine/base/primes.h"

#include "hash.h"

#include "hash-impl.h"

int hash_walk(hash_t *h, hash_walk_callback *cb, void *cbarg)
{
  int i;

  for (i = 0; i < h->nbins; i++)
  {
    node *n;
    node *next;

    for (n = h->bins[i]; n != NULL; n = next)
    {
      int r;

      next = n->next;

      r = cb(n->key, n->value, cbarg);
      if (r < 0)
        return r;
    }
  }

  return 0;
}
