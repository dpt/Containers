/* --------------------------------------------------------------------------
 *    Name: select.c
 * Purpose: Associative array implemented as a digital search tree
 * ----------------------------------------------------------------------- */

#include <assert.h>
#include <stdlib.h>

#include "base/errors.h"
#include "base/types.h"

#include "datastruct/item.h"

#include "datastruct/dstree.h"

#include "impl.h"

typedef struct dstree__select_args
{
  int     k;
  item_t *item;
}
dstree__select_args_t;

static error dstree__select_node(dstree__node_t *n,
                                 int             level,
                                 void           *opaque)
{
  dstree__select_args_t *args = opaque;

  NOT_USED(level);

  if (args->k-- == 0)
  {
    args->item = &n->item;
    return error_STOP_WALK;
  }

  return error_OK;
}

/* Walk the tree until the k'th leaf is encountered and return it. */
const item_t *dstree_select(dstree_t *t, int k)
{
  error                 err;
  dstree__select_args_t args;

  args.k    = k;
  args.item = NULL;

  err = dstree__walk_internal(t, dstree__select_node, &args);

  /* no errors save for the expected ones should happen here */
  assert(err == error_OK || err == error_STOP_WALK);

  return args.item;
}

