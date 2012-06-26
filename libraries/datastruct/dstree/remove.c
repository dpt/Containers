/* --------------------------------------------------------------------------
 *    Name: remove.c
 * Purpose: Associative array implemented as a digital search tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <string.h>

#include "base/memento/memento.h"

#include "datastruct/dstree.h"

#include "impl.h"

void dstree_remove(dstree_t *t, const void *key, size_t keylen)
{
  const unsigned char *ukey    = key;
  const unsigned char *ukeyend = ukey + keylen;
  int                  depth;
  dstree__node_t     **pn;
  dstree__node_t      *n;
  int                  dir;
  unsigned char        c;
  dstree__node_t     **pm;
  dstree__node_t      *m;

  depth = 0;
  c     = 0;

  for (pn = &t->root; (n = *pn); pn = &n->child[dir])
  {
    if (n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0)
      break; /* found */

    GET_NEXT_DIR(dir, ukey, ukeyend);
  }

  if (n == NULL)
    return; /* doesn't exist */

  /* pn now points to pointer to the doomed node */

  /* if the doomed node is a leaf node we just delete the parent pointer */
  if (IS_LEAF(n))
  {
    *pn = NULL;
    dstree__node_destroy(t, n);
    return;
  }

  /* otherwise we look for a leaf node that we can swap into the place of the
   * parent node */

  m = n;

  /* we need any leaf node downward of the to-be-removed node. so we just set
   * depth to zero here and hope that the binary values GET_NEXT_DIR()
   * returns cut us a random path through the tree. */
  ukey  = key;
  depth = 0;

  do
  {
    /* if there is no right child, then go left.
     * if there is no left child, then go right.
     * if both are present, fetch a 'random' value from GET_NEXT_DIR() and
     * choose. this means that we only call it when necessary. */

    if (m->child[1] == NULL)
      pm = &m->child[0];
    else if (m->child[0] == NULL)
      pm = &m->child[1];
    else
    {
      GET_NEXT_DIR(dir, ukey, ukeyend);
      pm = &m->child[dir];
      depth--; /* GET_NEXT_DIR increments depth, which we must compensate for */
    }

    m = *pm;
    depth++;
  }
  while (!IS_LEAF(m));

  /* found leaf node */

  *pm = NULL; /* detach */
  m->child[0] = n->child[0];
  m->child[1] = n->child[1];
  *pn = m; /* reattach */

  dstree__node_destroy(t, n);
}

