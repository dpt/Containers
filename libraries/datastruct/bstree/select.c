/* --------------------------------------------------------------------------
 *    Name: select.c
 * Purpose: Associative array implemented as a binary search tree
 * ----------------------------------------------------------------------- */

#include <assert.h>
#include <stdlib.h>

#include "base/errors.h"
#include "base/types.h"

#include "datastruct/bstree.h"
#include "datastruct/item.h"

#include "impl.h"

typedef struct bstree__select_args
{
  int     k;
  item_t *item;
}
bstree__select_args_t;

static error bstree__select_node(bstree__node_t *n,
                                 int             level,
                                 void           *opaque)
{
  bstree__select_args_t *args = opaque;

  NOT_USED(level);

  if (args->k-- == 0)
  {
    args->item = &n->item;
    return error_STOP_WALK;
  }

  return error_OK;
}

const item_t *bstree_select(bstree_t *t, int k)
{
  error                 err;
  bstree__select_args_t args;

  args.k    = k;
  args.item = NULL;

  err = bstree__walk_internal(t, bstree__select_node, &args);

  /* no errors save for the expected ones should happen here */
  assert(err == error_OK || err == error_STOP_WALK);

  return args.item;
}

