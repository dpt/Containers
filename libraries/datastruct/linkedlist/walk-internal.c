/* --------------------------------------------------------------------------
 *    Name: walk-internal.c
 * Purpose: Associative array implemented as a linked list
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/result.h"

#include "datastruct/linkedlist.h"

#include "impl.h"

result_t linkedlist__walk_internal(linkedlist_t                       *t,
                                   linkedlist__walk_internal_callback *cb,
                                   void                               *opaque)
{
  result_t            err;
  linkedlist__node_t *n;
  linkedlist__node_t *next;

  if (t == NULL)
    return result_OK;

  for (n = t->anchor; n; n = next)
  {
    next = n->next;

    err = cb(n, opaque);
    if (err)
      return err;
  }

  return result_OK;
}

