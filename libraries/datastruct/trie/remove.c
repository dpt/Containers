/* --------------------------------------------------------------------------
 *    Name: remove.c
 * Purpose: Associative array implemented as a trie
 * ----------------------------------------------------------------------- */

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "base/memento/memento.h"

#include "datastruct/trie.h"

#include "impl.h"

/* To remove, we search for the specified node recursively. If we hit a leaf
 * node and it's the correct key then we detach the key from its parent and
 * return 1 meaning 'fix up the tree'. Otherwise we return -1 meaning 'not
 * found'.
 *
 * On return from recursion we then test the return code. If negative we
 * return that code immediately. If positive we examine the tree to see if
 * it needs fixing up. A tree with a left or right NULL subtree pointer where
 * the other direction is not a subtree is invalid, and needs fixing up.
 *
 * A tree with a valid structure can terminate immediately and returns -2.
 */
static int trie__remove_node(trie_t        *t,
                             trie__node_t **pn,
                             const void    *key,
                             size_t         keylen,
                             int            depth)
{
  const unsigned char *ukey    = key;
  const unsigned char *ukeyend = ukey + keylen;
  trie__node_t        *n = *pn;
  trie__node_t        *m;

  assert(n != NULL);

  if (IS_LEAF(n))
  {
    if (!(n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0))
      return -1; /* not found */

    m = NULL; /* delete link from parent */
  }
  else
  {
    int           dir;
    int           rc;
    trie__node_t *left, *right;
    int           leafleft, leafright;

    dir = 0;
    if (ukey + (depth >> 3) < ukeyend)
      dir = (ukey[depth >> 3] >> (7 - (depth & 7))) & 1;

    if ((rc = trie__remove_node(t, &n->child[dir], key, keylen, depth + 1)) <= 0)
      return rc;

    left  = n->child[0];
    right = n->child[1];

    leafleft  = (right == NULL) && IS_LEAF(left);
    leafright = (left  == NULL) && IS_LEAF(right);
    if (!leafleft && !leafright)
      return -2; /* subtree is in a valid state */

    m = leafleft ? left : right;
    assert(m);
  }

  trie__node_destroy(t, n);
  *pn = m;

  return 1;
}

void trie_remove(trie_t *t, const void *key, size_t keylen)
{
  (void) trie__remove_node(t, &t->root, key, keylen, 0);
}
