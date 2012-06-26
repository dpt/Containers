/* --------------------------------------------------------------------------
 *    Name: breadthwalk-internal.c
 * Purpose: Associative array implemented as a digital search tree
 * ----------------------------------------------------------------------- */

#include <assert.h>
#include <stddef.h>

#include "base/errors.h"

#include "datastruct/queue.h"

#include "datastruct/critbit.h"

#include "impl.h"

error dstree__breadthwalk_internal(dstree_t                       *t,
                                   dstree__walk_internal_callback *cb,
                                   void                           *opaque)
{
  typedef struct nodedepth
  {
    dstree__node_t *node;
    int             depth;
  }
  nodedepth;

  error     err;
  queue_t  *queue;
  nodedepth nd;

  if (t == NULL)
    return error_OK;

  queue = queue_create(100, sizeof(nodedepth)); /* FIXME: 100 constant */
  if (queue == NULL)
    return error_OOM;

  assert(t->root);

  nd.node  = t->root;
  nd.depth = 0;

  err = queue_enqueue(queue, &nd);
  if (err)
    return err;

  while (!queue_empty(queue))
  {
    nodedepth ndc;

    err = queue_dequeue(queue, &nd);
    if (err)
      return err;

    err = cb(nd.node, nd.depth, opaque);
    if (err)
      return err;

    ndc.depth = nd.depth + 1;

    if (nd.node->child[0])
    {
      ndc.node = nd.node->child[0];
      err = queue_enqueue(queue, &ndc);
      if (err)
        return err;
    }
    if (nd.node->child[1])
    {
      ndc.node = nd.node->child[1];
      err = queue_enqueue(queue, &ndc);
      if (err)
        return err;
    }
  }

  queue_destroy(queue);

  return error_OK;
}

