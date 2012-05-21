/* --------------------------------------------------------------------------
 *    Name: count.c
 * Purpose: Hash
 * ----------------------------------------------------------------------- */

#include "hash.h"

#include "hash-impl.h"

int hash_count(hash_t *hash)
{
  return hash->count;
}
