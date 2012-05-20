/* --------------------------------------------------------------------------
 *    Name: primes.c
 * Purpose: Prime numbers
 * ----------------------------------------------------------------------- */

#include "utils.h"

#include "primes.h"

/* A selection of primes. */
static const int primes[] =
{
    17, 97, 173, 251, 337, 421, 503, 601, 683, 787, 881, 983,
};

int prime_nearest(int x)
{
  int i;

  for (i = 0; i < NELEMS(primes); i++)
    if (primes[i] >= x)
      return primes[i];

  return primes[i - 1]; /* choose the final value */
}

