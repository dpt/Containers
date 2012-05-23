/* select.c */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base/memento/memento.h"

#include "datastruct/queue.h"

#include "base/errors.h"
#include "utils/utils.h"
#include "string.h"
#include "keyval/string.h"

#include "datastruct/dstree.h"

#include "impl.h"

/* ----------------------------------------------------------------------- */

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

/* ----------------------------------------------------------------------- */

