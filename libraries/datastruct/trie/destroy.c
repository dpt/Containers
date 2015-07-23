/* --------------------------------------------------------------------------
 *    Name: destroy.c
 * Purpose: Associative array implemented as a trie
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "base/types.h"

#include "datastruct/trie.h"

#include "impl.h"

static result_t trie__destroy_node(trie__node_t *n, int level, void *opaque)
{
  NOT_USED(level);

  trie__node_destroy(opaque, n);

  return result_OK;
}

void trie_destroy(trie_t *t)
{
  (void) trie__walk_internal(t, trie_WALK_POST_ORDER, trie__destroy_node, t);

  free(t);
}
