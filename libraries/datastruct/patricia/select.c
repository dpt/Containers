/* --------------------------------------------------------------------------
 *    Name: select.c
 * Purpose: Associative array implemented as a PATRICIA tree
 * ----------------------------------------------------------------------- */

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "base/result.h"
#include "base/types.h"

#include "datastruct/item.h"

#include "datastruct/dstree.h"

#include "impl.h"

typedef struct patricia__select_args
{
  int     k;
  item_t *item;
}
patricia__select_args_t;

static result_t patricia__select_node(patricia__node_t *n,
                                      int               level,
                                      void             *opaque)
{
  patricia__select_args_t *args = opaque;

  NOT_USED(level);

  if (args->k-- == 0)
  {
    args->item = &n->item;
    return result_STOP_WALK;
  }

  return result_OK;
}

/* Walk the tree until the k'th leaf is encountered and return it. */
const item_t *patricia_select(patricia_t *t, int k)
{
  result_t                err;
  patricia__select_args_t args;

  args.k    = k;
  args.item = NULL;

  err = patricia__walk_internal(t,
                                patricia_WALK_LEAVES,
                                patricia__select_node,
                                &args);

  /* no errors save for the expected ones should happen here */
  assert(err == result_OK || err == result_STOP_WALK);

  return args.item;
}

