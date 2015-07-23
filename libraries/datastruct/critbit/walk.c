/* --------------------------------------------------------------------------
 *    Name: walk.c
 * Purpose: Associative array implemented as a critbit tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>

#include "base/result.h"

#include "datastruct/critbit.h"

#include "impl.h"

static result_t critbit__walk_in_order(const critbit__node_t *n,
                                       int                    level,
                                       critbit_walk_callback *cb,
                                       void                  *opaque)
{
  result_t            err;
  critbit__extnode_t *e;

  if (n == NULL)
    return result_OK;

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

  return result_OK;
}

result_t critbit_walk(const critbit_t       *t,
                      critbit_walk_callback *cb,
                      void                  *opaque)
{
  if (t == NULL)
    return result_OK;

  return critbit__walk_in_order(t->root, 0, cb, opaque);
}

