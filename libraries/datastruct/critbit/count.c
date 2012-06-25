/* --------------------------------------------------------------------------
 *    Name: count.c
 * Purpose: Associative array implemented as a critbit tree
 * ----------------------------------------------------------------------- */

#include "datastruct/critbit.h"

#include "impl.h"

int critbit_count(critbit_t *t)
{
  return t->intcount + t->extcount;
}

