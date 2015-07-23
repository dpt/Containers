/* --------------------------------------------------------------------------
 *    Name: destroy.c
 * Purpose: Associative array implemented as a critbit tree
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"
#include "base/result.h"
#include "base/types.h"

#include "datastruct/critbit.h"

#include "impl.h"

static result_t critbit__destroy_node(critbit__node_t *n,
                                      int              level,
                                      void            *opaque)
{
  NOT_USED(level);

  if (IS_EXTERNAL(n))
    critbit__extnode_destroy(opaque, FROM_STORE(n));
  else
    critbit__node_destroy(opaque, n);

  return result_OK;
}

void critbit_destroy(critbit_t *t)
{
  (void) critbit__walk_internal(t,
                                critbit_WALK_POST_ORDER |
                                critbit_WALK_LEAVES     |
                                critbit_WALK_BRANCHES,
                                critbit__destroy_node,
                                t);

  free(t);
}

