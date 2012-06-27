/* --------------------------------------------------------------------------
 *    Name: count.c
 * Purpose: Associative array implemented as an ordered array
 * ----------------------------------------------------------------------- */

#include "datastruct/orderedarray.h"

#include "impl.h"

int orderedarray_count(orderedarray_t *t)
{
  return t->nelems;
}

