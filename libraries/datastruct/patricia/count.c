/* --------------------------------------------------------------------------
 *    Name: count.c
 * Purpose: Associative array implemented as a PATRICIA tree
 * ----------------------------------------------------------------------- */

#include "datastruct/critbit.h"

#include "impl.h"

int patricia_count(patricia_t *t)
{
  return t->count;
}

