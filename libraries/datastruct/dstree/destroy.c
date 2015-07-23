/* --------------------------------------------------------------------------
 *    Name: destroy.c
 * Purpose: Associative array implemented as a digital search tree
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "base/types.h"

#include "datastruct/dstree.h"

#include "impl.h"

static result_t dstree__destroy_node(dstree__node_t *n,
                                     int             level,
                                     void           *opaque)
{
  NOT_USED(level);

  dstree__node_destroy(opaque, n);

  return result_OK;
}

void dstree_destroy(dstree_t *t)
{
  (void) dstree__walk_internal_post(t, dstree__destroy_node, t);

  free(t);
}

