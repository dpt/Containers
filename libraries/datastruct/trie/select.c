/* select.c */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base/memento/memento.h"

#include "base/errors.h"
#include "utils/utils.h"
#include "datastruct/queue.h"
#include "string.h"
#include "keyval/string.h"

#include "datastruct/trie.h"
#include "impl.h"

/* ----------------------------------------------------------------------- */

typedef struct trie__select_args
{
  int     k;
  item_t *item;
}
trie__select_args_t;

static error trie__select_node(trie__node_t *n,
                               int           level,
                               void         *opaque)
{
  trie__select_args_t *args = opaque;

  NOT_USED(level);

  if (args->k-- == 0)
  {
    args->item = &n->item;
    return error_STOP_WALK;
  }

  return error_OK;
}

const item_t *trie_select(trie_t *t, int k)
{
  error               err;
  trie__select_args_t args;

  args.k    = k;
  args.item = NULL;

  err = trie__walk_internal(t,
                            trie_WALK_IN_ORDER | trie_WALK_LEAVES,
                            trie__select_node,
                            &args);

  /* no errors save for the expected ones should happen here */
  assert(err == error_OK || err == error_STOP_WALK);

  return args.item;
}

/* ----------------------------------------------------------------------- */

