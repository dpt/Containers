/* --------------------------------------------------------------------------
 *    Name: count.c
 * Purpose: Associative array implemented as a binary search tree
 * ----------------------------------------------------------------------- */

#include "datastruct/bstree.h"

#include "impl.h"

int bstree_count(bstree_t *t)
{
  return t->count;
}

