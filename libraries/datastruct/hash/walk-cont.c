/* --------------------------------------------------------------------------
 *    Name: walk-cont.c
 * Purpose: Associative array implemented as a hash
 * ----------------------------------------------------------------------- */

#include <assert.h>
#include <stdlib.h>

#include "base/memento/memento.h"

#include "base/errors.h"

#include "datastruct/hash.h"

#include "impl.h"

error hash_walk_continuation(hash_t      *h,
                             int          continuation,
                             int         *nextcontinuation,
                             const void **key,
                             const void **value)
{
  unsigned int  bin;
  unsigned int  item;
  int           i;
  hash__node_t *n;
  hash__node_t *next = NULL;

  if (continuation == -1) /* previous iteration was the last element */
    return error_HASH_END;

  /* The continuation value is treated as a pair of 16-bit fields, the top
   * half being the bin and the bottom half being the node within the bin. */

  bin  = ((unsigned int) continuation & 0xffff0000) >> 16;
  item = ((unsigned int) continuation & 0x0000ffff) >> 0;

  if (bin >= (unsigned int) h->nbins)
    return error_HASH_BAD_CONT; /* invalid continuation value */

  /* if we're starting off, scan forward to the first occupied bin */

  if (continuation == 0)
  {
    while (h->bins[bin] == NULL)
      bin++;

    if (bin == (unsigned int) h->nbins)
      return error_HASH_END; /* all bins were empty */
  }

  i = 0; /* node counter */

  for (n = h->bins[bin]; n; n = next)
  {
    next = n->next;

    if ((unsigned int) i == item)
      break;

    i++;
  }

  if (n == NULL) /* invalid continuation value */
    return error_HASH_BAD_CONT;

  *key   = n->item.key;
  *value = n->item.value;

  /* form the continuation value and return it */

  /* a chain this long would be very odd, but ... */
  assert(i + 1 <= 0xffff);

  if (next)
  {
    *nextcontinuation = (bin << 16) | (i + 1); /* current bin, next node */
    return error_OK;
  }

  /* scan forward to the next occupied bin */

  while (++bin < (unsigned int) h->nbins)
    if (h->bins[bin])
    {
      *nextcontinuation = bin << 16; /* next occupied bin, first node */
      return error_OK;
    }

  *nextcontinuation = -1; /* ran out of bins - remember for next iter */

  return error_OK;
}
