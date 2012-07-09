/* --------------------------------------------------------------------------
 *    Name: node-create.c
 * Purpose: Associative array implemented as a trie
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <stdlib.h>

#include "base/memento/memento.h"

#include "datastruct/trie.h"

#include "impl.h"

trie__node_t *trie__node_create(trie_t     *t,
                                const void *key,
                                size_t      keylen,
                                const void *value)
{
  trie__node_t *n;

  n = malloc(sizeof(*n));
  if (n == NULL)
    return NULL;

  n->child[0]    = NULL;
  n->child[1]    = NULL;
  n->item.key    = key;
  n->item.keylen = keylen;
  n->item.value  = value;

  t->count++;

  return n;
}
