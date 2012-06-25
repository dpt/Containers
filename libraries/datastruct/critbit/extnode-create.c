/* --------------------------------------------------------------------------
 *    Name: extnode-create.c
 * Purpose: Associative array implemented as a critbit tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <stdlib.h>

#include "base/memento/memento.h"

#include "datastruct/critbit.h"

#include "impl.h"

critbit__extnode_t *critbit__extnode_create(critbit_t  *t,
                                            const void *key,
                                            size_t      keylen,
                                            const void *value)
{
  critbit__extnode_t *n;

  /* We assume that returned malloc blocks are aligned to at least a two-byte
   * boundary, leaving us the bottom bit spare to use as as a node type flag.
   */

  n = malloc(sizeof(*n));
  if (n == NULL)
    return NULL;

  n->item.key    = key;
  n->item.keylen = keylen;
  n->item.value  = value;

  t->extcount++;

  return n;
}

