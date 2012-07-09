/* --------------------------------------------------------------------------
 *    Name: count.c
 * Purpose: Associative array implemented as a trie
 * ----------------------------------------------------------------------- */

#include "datastruct/trie.h"

#include "impl.h"

int trie_count(trie_t *t)
{
  return t->count;
}
