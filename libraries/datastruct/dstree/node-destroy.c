/* --------------------------------------------------------------------------
 *    Name: node-destroy.c
 * Purpose: Associative array implemented as a digital search tree
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "datastruct/critbit.h"

#include "impl.h"

void dstree__node_destroy(dstree_t *t, dstree__node_t *n)
{
  dstree__node_clear(t, n);

  free(n);

  t->count--;
}

