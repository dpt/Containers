/* --------------------------------------------------------------------------
 *    Name: count.c
 * Purpose: Associative array implemented as a hash
 * ----------------------------------------------------------------------- */

#include "datastruct/hash.h"

#include "impl.h"

int hash_count(hash_t *hash)
{
  return hash->count;
}
