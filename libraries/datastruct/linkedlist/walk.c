/* --------------------------------------------------------------------------
 *    Name: walk.c
 * Purpose: Associative array implemented as a linked list
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/errors.h"

#include "datastruct/linkedlist.h"

#include "impl.h"

error linkedlist_walk(const linkedlist_t       *t,
                      linkedlist_walk_callback *cb,
                      void                     *opaque)
{
  error               err;
  linkedlist__node_t *n;
  linkedlist__node_t *next;

  if (t == NULL)
    return error_OK;

  for (n = t->anchor; n; n = next)
  {
    next = n->next;

    err = cb(&n->item, opaque);
    if (err)
      return err;
  }

  return error_OK;
}

