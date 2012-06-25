/* --------------------------------------------------------------------------
 *    Name: walk-internal.c
 * Purpose: Associative array implemented as a critbit tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <stdlib.h>

#include "base/errors.h"

#include "datastruct/critbit.h"

#include "impl.h"

// unlike the walking methods in other data structures, we cannot callback on
// the current node as any non-leaf node does not hold an item
//
static error critbit__walk_internal_in_order(critbit__node_t                 *n,
                                             critbit_walk_flags               flags,
                                             int                              level,
                                             critbit__walk_internal_callback *cb,
                                             void                            *opaque)
{
  error err;
  int   i;

  if (n == NULL)
    return error_OK;

  if (IS_EXTERNAL(n))
  {
    if ((flags & critbit_WALK_LEAVES) != 0)
    {
      err = cb(n, level, opaque);
      if (err)
        return err;
    }
  }
  else
  {
    for (i = 0; i < 2; i++)
    {
      err = critbit__walk_internal_in_order(n->child[i], flags, level + 1, cb, opaque);
      if (err)
        return err;

      if (i == 0 && (flags & critbit_WALK_BRANCHES) != 0) /* inbetween */
      {
        err = cb(n, level, opaque);
        if (err)
          return err;
      }
    }
  }

  return error_OK;
}

static error critbit__walk_internal_pre_order(critbit__node_t                 *n,
                                              critbit_walk_flags               flags,
                                              int                              level,
                                              critbit__walk_internal_callback *cb,
                                              void                            *opaque)
{
  error err;
  int   i;

  if (n == NULL)
    return error_OK;

  if (IS_EXTERNAL(n))
  {
    if ((flags & critbit_WALK_LEAVES) != 0)
    {
      err = cb(n, level, opaque);
      if (err)
        return err;
    }
  }
  else
  {
    /* self */
    if ((flags & critbit_WALK_BRANCHES) != 0)
    {
      err = cb(n, level, opaque);
      if (err)
        return err;
    }

    for (i = 0; i < 2; i++)
    {
      err = critbit__walk_internal_pre_order(n->child[i], flags, level + 1, cb, opaque);
      if (err)
        return err;
    }
  }

  return error_OK;
}

static error critbit__walk_internal_post_order(critbit__node_t                 *n,
                                               critbit_walk_flags               flags,
                                               int                              level,
                                               critbit__walk_internal_callback *cb,
                                               void                            *opaque)
{
  error err;
  int   i;

  if (n == NULL)
    return error_OK;

  if (IS_EXTERNAL(n))
  {
    if ((flags & critbit_WALK_LEAVES) != 0)
    {
      err = cb(n, level, opaque);
      if (err)
        return err;
    }
  }
  else
  {
    for (i = 0; i < 2; i++)
    {
      err = critbit__walk_internal_post_order(n->child[i], flags, level + 1, cb, opaque);
      if (err)
        return err;
    }

    /* self */
    if ((flags & critbit_WALK_BRANCHES) != 0)
    {
      err = cb(n, level, opaque);
      if (err)
        return err;
    }
  }

  return error_OK;
}

error critbit__walk_internal(critbit_t                       *t,
                             critbit_walk_flags               flags,
                             critbit__walk_internal_callback *cb,
                             void                            *opaque)
{
  error (*walker)(critbit__node_t                 *n,
                  critbit_walk_flags               flags,
                  int                              level,
                  critbit__walk_internal_callback *cb,
                  void                            *opaque);

  if (t == NULL)
    return error_OK;

  switch (flags & critbit_WALK_ORDER_MASK)
  {
  default:
  case critbit_WALK_IN_ORDER:
    walker = critbit__walk_internal_in_order;
    break;

  case critbit_WALK_PRE_ORDER:
    walker = critbit__walk_internal_pre_order;
    break;

  case critbit_WALK_POST_ORDER:
    walker = critbit__walk_internal_post_order;
    break;
  }

  return walker(t->root, flags, 0, cb, opaque);
}

