/* --------------------------------------------------------------------------
 *    Name: walk.c
 * Purpose: Associative array implemented as a PATRICIA tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>

#include "base/errors.h"

#include "datastruct/patricia.h"

#include "impl.h"

static error patricia__walk_in_order(patricia__node_t       *n,
                                     int                     level,
                                     patricia_walk_callback *cb,
                                     void                   *opaque)
{
  error err;
  int   i;

  if (n == NULL)
    return error_OK;

  for (i = 0; i < 2; i++)
  {
    /* does this child point to a leaf? */
    if (n->child[i] == NULL)
      continue;

    if (n->child[i]->bit <= n->bit)
    {
      err = cb(n->child[i]->item.key, n->child[i]->item.value, level, opaque);
      if (err)
        return err;
    }
    else
    {
      err = patricia__walk_in_order(n->child[i], level + 1, cb, opaque);
      if (err)
        return err;
    }
  }

  return error_OK;
}

error patricia_walk(const patricia_t       *t,
                    patricia_walk_callback *cb,
                    void                   *opaque)
{
  error (*walker)(patricia__node_t       *n,
                  int                     level,
                  patricia_walk_callback *cb,
                  void                   *opaque);

  if (t == NULL)
    return error_OK;

  walker = patricia__walk_in_order;

  return walker(t->root, 0, cb, opaque);
}

