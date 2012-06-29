/* --------------------------------------------------------------------------
 *    Name: utils.c
 * Purpose: Utility functions common across data structures
 * ----------------------------------------------------------------------- */

#include <assert.h>
#include <stddef.h>

#include "utils/utils.h"

int keydiffbit(const unsigned char *key1, size_t key1len,
               const unsigned char *key2, size_t key2len)
{
  const unsigned char *longest;
  const unsigned char *shortest;
  size_t               commonlen;
  const unsigned char *longest_base;
  int                  mask;
  int                  offset;

  /* sort out which keys are longest and shortest */
  if (key1len >= key2len)
  {
    longest   = key1;
    shortest  = key2;
    commonlen = key2len;
  }
  else
  {
    longest   = key2;
    shortest  = key1;
    commonlen = key1len;
  }

  longest_base = longest;

  /* find differing bytes in the common span of the keys */
  for (; commonlen; commonlen--)
    if ((mask = *longest++ ^ *shortest++) != 0)
      break;

  /* we've now found a difference, or run out of common bytes */

  /* if we ran out of bytes to compare look at longest's suffix */
  if (commonlen == 0)
  {
    const unsigned char *longest_end;

    if (key1len == key2len) /* (might not happen in practice) */
      return -1; /* keys are the same length - there can be no difference */

    /* we could arrive here if the shortest key was zero bytes long and the
     * longest key was positive bytes long. 'mask' may be unset. */

    if (key1len >= key2len)
      longest_end = key1 + key1len;
    else
      longest_end = key2 + key2len;

    assert(longest != longest_end);

    /* look for a non-zero byte */
    /* note that this is a do-while loop to appease clang's analysis. it says
     * that mask might be getting used uninitialised and draws many pretty
     * arrows, but i don't follow its reasoning. */
    do
      if ((mask = *longest++) != 0)
        break;
    while (longest < longest_end);

    if (longest == longest_end && mask == 0)
      return -1; /* we ran out of bytes */
  }

  offset = (longest - 1 - longest_base) * 8;

  /* 128, 64, 32, ... => 0, 1, 2, ... */
  if (mask & 128) return offset + 0;
  if (mask &  64) return offset + 1;
  if (mask &  32) return offset + 2;
  if (mask &  16) return offset + 3;
  if (mask &   8) return offset + 4;
  if (mask &   4) return offset + 5;
  if (mask &   2) return offset + 6;
  return offset + 7;
}

int iszero(const void *k, size_t len)
{
  const unsigned char *start = k;
  const unsigned char *end   = start + len;

  while (start < end)
    if (*start++)
      return 0;

  return 1;
}

