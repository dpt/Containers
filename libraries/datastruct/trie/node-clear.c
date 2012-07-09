/* --------------------------------------------------------------------------
 *    Name: node-destroy.c
 * Purpose: Associative array implemented as a trie
 * ----------------------------------------------------------------------- */

#include "datastruct/trie.h"

#include "impl.h"

void trie__node_clear(trie_t *t, trie__node_t *n)
{
  if (t->destroy_key)
    t->destroy_key((void *) n->item.key); /* must cast away const */
  if (t->destroy_value)
    t->destroy_value((void *) n->item.value);
}
