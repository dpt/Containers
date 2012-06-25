/* --------------------------------------------------------------------------
 *    Name: remove.c
 * Purpose: Associative array implemented as a binary search tree
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "datastruct/bstree.h"

#include "impl.h"

void bstree_remove(bstree_t *t, const void *key)
{
  bstree__node_t **pn;
  bstree__node_t  *n;

  pn = &t->root;
  n  = *pn;
  while (n)
  {
    int d;

    d = t->compare(key, n->item.key);
    if (d == 0)
      break;

    pn = &n->child[d < 0 ? 0 : 1];
    n  = *pn;
  }

  if (n == NULL)
    return; /* not found */

  bstree__node_destroy_item(t, n);

  /* case 1: node has no children */
  if (n->child[0] == NULL && n->child[1] == NULL)
  {
    /* set parent to NULL */
    *pn = NULL;
  }
  /* case 2: node has just one child */
  else if (n->child[0] == NULL || n->child[1] == NULL)
  {
    /* point parent to grandchild */
    *pn = n->child[0] ? n->child[0] : n->child[1];
  }
  /* case 3: node has both children */
  else
  {
    bstree__node_t **pmin;
    bstree__node_t  *min;

    /* find minimum node in right subtree */
    pmin = &n->child[1];
    min  = *pmin;
    while (min->child[0])
    {
      pmin = &min->child[0];
      min  = *pmin;
    }

    /* take minimum node's item to replace existing item */
    n->item = min->item;

    /* minimum is now a duplicate: remove it */
    *pmin = min->child[1]; /* right child, or NULL */

    n = min;
  }

  free(n);

  t->count--;
}

