/* --------------------------------------------------------------------------
 *    Name: count.c
 * Purpose: Associative array implemented as a linked list
 * ----------------------------------------------------------------------- */

#include "datastruct/linkedlist.h"

#include "impl.h"

int linkedlist_count(linkedlist_t *t)
{
  return t->count;
}

