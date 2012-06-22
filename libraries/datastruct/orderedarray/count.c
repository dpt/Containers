/* --------------------------------------------------------------------------
 *    Name: count.c
 * Purpose: Ordered array
 * ----------------------------------------------------------------------- */

#include "datastruct/orderedarray.h"

#include "impl.h"

int orderedarray_count(orderedarray_t *t)
{
  return t->nelems;
}

