/* --------------------------------------------------------------------------
 *    Name: remove.c
 * Purpose: Associative array implemented as a critbit tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <string.h>

#include "base/memento/memento.h"

#include "datastruct/critbit.h"

#include "impl.h"

void critbit_remove(critbit_t *t, const void *key, size_t keylen)
{
  const unsigned char *ukey    = key;
  const unsigned char *ukeyend = ukey + keylen;
  critbit__node_t     *n;
  critbit__node_t    **wherem;
  critbit__node_t    **wheren;
  int                  dir;
  critbit__extnode_t  *e;
  critbit__node_t     *lastn;

  n = t->root;

  wherem = NULL;
  wheren = &t->root;

  if (t->root == NULL)
    return; /* empty tree */

  while (IS_INTERNAL(n))
  {
    wherem = wheren;

    dir = GET_DIR(ukey, ukeyend, n->byte, n->otherbits);
    wheren = &n->child[dir];

    lastn = n;
    n = *wheren;
  }

  e = FROM_STORE(n);

  if (!(e->item.keylen == keylen && memcmp(e->item.key, key, keylen) == 0))
    return; /* not found */

  critbit__extnode_destroy(t, e);

  if (wherem == NULL)
  {
    t->root = NULL;
  }
  else
  {
    *wherem = lastn->child[1 - dir];

    critbit__node_destroy(t, lastn);
  }
}

