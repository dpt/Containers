/* --------------------------------------------------------------------------
 *    Name: breadthwalk-internal.c
 * Purpose: Associative array implemented as a trie
 * ----------------------------------------------------------------------- */

#include <assert.h>
#include <stddef.h>

#include "base/result.h"

#include "datastruct/queue.h"

#include "datastruct/trie.h"

#include "impl.h"

result_t trie__breadthwalk_internal(trie_t                       *t,
                                    trie_walk_flags               flags,
                                    trie__walk_internal_callback *cb,
                                    void                         *opaque)
{
  typedef struct nodedepth
  {
    trie__node_t *node;
    int           depth;
  }
  nodedepth;

  result_t  err;
  queue_t  *queue;
  nodedepth nd;

  if (t == NULL)
    return result_OK;

  queue = queue_create(100, sizeof(nodedepth)); /* FIXME: 100 constant */
  if (queue == NULL)
    return result_OOM;

  assert(t->root);

  nd.node  = t->root;
  nd.depth = 0;

  err = queue_enqueue(queue, &nd);
  if (err)
    return err;

  while (!queue_empty(queue))
  {
    int       leaf;
    nodedepth ndc;

    err = queue_dequeue(queue, &nd);
    if (err)
      return err;

    leaf = IS_LEAF(nd.node);

    // this actually means /visit/ leaves or branches we still walk
    // the entire structure and queue up child elements for visiting
    if (((flags & trie_WALK_LEAVES)   != 0 && leaf) ||
        ((flags & trie_WALK_BRANCHES) != 0 && !leaf))
    {
      err = cb(nd.node, nd.depth, opaque);
      if (err)
        return err;
    }

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

  return result_OK;
}
