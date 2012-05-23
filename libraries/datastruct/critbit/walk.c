/* walk.c -- associative array implemented as critbit tree */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base/memento/memento.h"

#include "base/types.h"
#include "string.h"
#include "keyval/string.h"

#include "datastruct/critbit.h"

#include "impl.h"

/* ----------------------------------------------------------------------- */

static error critbit__walk_in_order(const critbit__node_t *n,
                                    int                    level,
                                    critbit_walk_callback *cb,
                                    void                  *opaque)
{
  error               err;
  critbit__extnode_t *e;

  if (n == NULL)
    return error_OK;

  if (IS_EXTERNAL(n))
  {
    e = FROM_STORE(n);

    err = cb(e->item.key, e->item.value, level, opaque);
    if (err)
      return err;
  }
  else
  {
    err = critbit__walk_in_order(n->child[0], level + 1, cb, opaque);
    if (err)
      return err;

    err = critbit__walk_in_order(n->child[1], level + 1, cb, opaque);
    if (err)
      return err;
  }

  return error_OK;
}

error critbit_walk(const critbit_t       *t,
                   critbit_walk_callback *cb,
                   void                  *opaque)
{
  if (t == NULL)
    return error_OK;

  return critbit__walk_in_order(t->root, 0, cb, opaque);
}

/* ----------------------------------------------------------------------- */
