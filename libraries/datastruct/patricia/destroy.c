/* --------------------------------------------------------------------------
 *    Name: destroy.c
 * Purpose: Associative array implemented as a PATRICIA tree
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "base/types.h"

#include "datastruct/patricia.h"

#include "impl.h"

static error patricia__destroy_node(patricia__node_t *n,
                                    int               level,
                                    void             *opaque)
{
  NOT_USED(level);

  patricia__node_destroy(opaque, n);

  return error_OK;
}

void patricia_destroy(patricia_t *t)
{
  (void) patricia__walk_internal(t,
                                 patricia_WALK_POST_ORDER |
                                 patricia_WALK_BRANCHES,
                                 patricia__destroy_node,
                                 t);

  free(t);
}

