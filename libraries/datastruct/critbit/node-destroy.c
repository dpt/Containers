/* --------------------------------------------------------------------------
 *    Name: node-destroy.c
 * Purpose: Associative array implemented as a critbit tree
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "datastruct/critbit.h"

#include "impl.h"

void critbit__node_destroy(critbit_t *t, critbit__node_t *n)
{
  free(n);

  t->intcount--;
}

