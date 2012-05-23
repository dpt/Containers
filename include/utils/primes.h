/* --------------------------------------------------------------------------
 *    Name: primes.h
 * Purpose: Prime numbers
 * ----------------------------------------------------------------------- */

#ifndef PRIMES_H
#define PRIMES_H

/* Returns the nearest prime number to 'x' from a greatly reduced range.
 * Intended as a cache of values for use when sizing data structures. */
int prime_nearest(int x);

#endif /* PRIMES_H */

