/* --------------------------------------------------------------------------
 *    Name: node-destroy.c
 * Purpose: Associative array implemented as a trie
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "datastruct/trie.h"

#include "impl.h"

void trie__node_destroy(trie_t *t, trie__node_t *n)
{
  trie__node_clear(t, n);

  free(n);

  t->count--;
}
