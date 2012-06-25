/* --------------------------------------------------------------------------
 *    Name: extnode-destroy.c
 * Purpose: Associative array implemented as a critbit tree
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"
#include "base/errors.h"

#include "datastruct/critbit.h"

#include "impl.h"

void critbit__extnode_clear(critbit_t *t, critbit__extnode_t *n)
{
  if (t->destroy_key && n->item.key)
    t->destroy_key((void *) n->item.key); /* must cast away const */
  if (t->destroy_value && n->item.value)
    t->destroy_value((void *) n->item.value);
}

void critbit__extnode_destroy(critbit_t *t, critbit__extnode_t *n)
{
  critbit__extnode_clear(t, n);

  free(n);

  t->extcount--;
}

