/* --------------------------------------------------------------------------
 *    Name: node-destroy.c
 * Purpose: Associative array implemented as a PATRICIA tree
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "datastruct/patricia.h"

#include "impl.h"

void patricia__node_destroy(patricia_t *t, patricia__node_t *n)
{
  patricia__node_clear(t, n);

  free(n);

  t->count--;
}

