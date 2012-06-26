/* --------------------------------------------------------------------------
 *    Name: count.c
 * Purpose: Associative array implemented as a digital search tree
 * ----------------------------------------------------------------------- */

#include "datastruct/critbit.h"

#include "impl.h"

int dstree_count(dstree_t *t)
{
  return t->count;
}

