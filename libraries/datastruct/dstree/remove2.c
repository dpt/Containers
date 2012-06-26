/* --------------------------------------------------------------------------
 *    Name: remove2.c
 * Purpose: Associative array implemented as a digital search tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <string.h>

#include "base/memento/memento.h"

#include "datastruct/dstree.h"

#include "impl.h"

void dstree_remove2(dstree_t *t, const void *key, size_t keylen)
{
  const unsigned char *ukey    = key;
  const unsigned char *ukeyend = ukey + keylen;
  int                  depth;
  dstree__node_t     **pn;
  dstree__node_t      *n;
  unsigned char        c = 0;
  int                  dir;
  dstree__node_t      *root;
  dstree__node_t      *side;

  depth = 0;

  for (pn = &t->root; (n = *pn); pn = &n->child[dir])
  {
    if (n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0)
      break; /* found */

    GET_NEXT_DIR(dir, ukey, ukeyend);
  }

  if (n == NULL)
    return; /* not found */

  /* pick a random child. if that child's absent then pick the other */
  GET_NEXT_DIR(dir, ukey, ukeyend);
  if (n->child[dir] == NULL)
    dir = !dir;

  root = n->child[dir];  /* new tree root */
  side = n->child[!dir]; /* detached other side */

  /* detach root node */
  *pn = NULL;

  while (root)
  {
    int             newdir;
    dstree__node_t *nextroot, *nextside;

    GET_NEXT_DIR(newdir, ukey, ukeyend);
    if (n->child[newdir] == NULL)
      newdir = !newdir;

    nextroot = root->child[newdir];
    nextside = root->child[!newdir];

    root->child[dir]  = nextroot;
    root->child[!dir] = side;

    dir = newdir;
    root = nextroot;
    side = nextside;
  }

  dstree__node_destroy(t, n);
}

