/* --------------------------------------------------------------------------
 *    Name: walk-internal.c
 * Purpose: Associative array implemented as a PATRICIA tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <stdlib.h>

#include "base/result.h"

#include "datastruct/patricia.h"

#include "impl.h"

static result_t patricia__walk_internal_in_order(patricia__node_t                 *n,
                                                 patricia_walk_flags               flags,
                                                 int                               level,
                                                 patricia__walk_internal_callback *cb,
                                                 void                             *opaque)
{
  result_t err;
  int      i;

  if (n == NULL)
    return result_OK;

  for (i = 0; i < 2; i++)
  {
    if (n->child[i] == NULL)
      continue;

    /* does this child point to a leaf? */
    if (n->child[i]->bit <= n->bit)
    {
      if ((flags & patricia_WALK_LEAVES) != 0)
      {
        err = cb(n->child[i], level, opaque);
        if (err)
          return err;
      }
    }
    else
    {
      err = patricia__walk_internal_in_order(n->child[i], flags, level + 1, cb, opaque);
      if (err)
        return err;
    }

    /* self */
    if (i == 0 && (flags & patricia_WALK_BRANCHES) != 0) /* inbetween */
    {
      err = cb(n, level, opaque);
      if (err)
        return err;
    }
  }

  return result_OK;
}

static result_t patricia__walk_internal_pre_order(patricia__node_t                 *n,
                                                  patricia_walk_flags               flags,
                                                  int                               level,
                                                  patricia__walk_internal_callback *cb,
                                                  void                             *opaque)
{
  result_t err;
  int      i;

  if (n == NULL)
    return result_OK;

  /* self */
  if ((flags & patricia_WALK_BRANCHES) != 0)
  {
    err = cb(n, level, opaque);
    if (err)
      return err;
  }

  for (i = 0; i < 2; i++)
  {
    if (n->child[i] == NULL)
      continue;

    /* does this child point to a leaf? */
    if (n->child[i]->bit <= n->bit)
    {
      if ((flags & patricia_WALK_LEAVES) != 0)
      {
        err = cb(n->child[i], level, opaque);
        if (err)
          return err;
      }
    }
    else
    {
      err = patricia__walk_internal_pre_order(n->child[i], flags, level + 1, cb, opaque);
      if (err)
        return err;
    }
  }

  return result_OK;
}

static result_t patricia__walk_internal_post_order(patricia__node_t                 *n,
                                                   patricia_walk_flags               flags,
                                                   int                               level,
                                                   patricia__walk_internal_callback *cb,
                                                   void                             *opaque)
{
  result_t err;
  int      i;

  if (n == NULL)
    return result_OK;

  for (i = 0; i < 2; i++)
  {
    if (n->child[i] == NULL)
      continue;

    /* does this child point to a leaf? */
    if (n->child[i]->bit <= n->bit)
    {
      if ((flags & patricia_WALK_LEAVES) != 0)
      {
        err = cb(n->child[i], level, opaque);
        if (err)
          return err;
      }
    }
    else
    {
      err = patricia__walk_internal_post_order(n->child[i], flags, level + 1, cb, opaque);
      if (err)
        return err;
    }
  }

  /* self */
  if ((flags & patricia_WALK_BRANCHES) != 0)
  {
    err = cb(n, level, opaque);
    if (err)
      return err;
  }

  return result_OK;
}

result_t patricia__walk_internal(patricia_t                       *t,
                                 patricia_walk_flags               flags,
                                 patricia__walk_internal_callback *cb,
                                 void                             *opaque)
{
  result_t (*walker)(patricia__node_t                 *n,
                     patricia_walk_flags               flags,
                     int                               level,
                     patricia__walk_internal_callback *cb,
                     void                             *opaque);

  if (t == NULL)
    return result_OK;

  switch (flags & patricia_WALK_ORDER_MASK)
  {
  default:
  case patricia_WALK_IN_ORDER:
    walker = patricia__walk_internal_in_order;
    break;

  case patricia_WALK_PRE_ORDER:
    walker = patricia__walk_internal_pre_order;
    break;

  case patricia_WALK_POST_ORDER:
    walker = patricia__walk_internal_post_order;
    break;
  }

  return walker(t->root, flags, 0, cb, opaque);
}

