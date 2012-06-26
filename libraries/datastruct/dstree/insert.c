/* --------------------------------------------------------------------------
 *    Name: insert.c
 * Purpose: Associative array implemented as a digital search tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <string.h>

#include "base/errors.h"

#include "datastruct/dstree.h"

#include "impl.h"

error dstree_insert(dstree_t   *t,
                    const void *key,
                    size_t      keylen,
                    const void *value)
{
  const unsigned char *ukey    = key;
  const unsigned char *ukeyend = ukey + keylen;
  int                  depth;
  dstree__node_t     **pn;
  dstree__node_t      *n;
  int                  dir;
  unsigned char        c = 0;

  depth = 0;

  for (pn = &t->root; (n = *pn); pn = &n->child[dir])
  {
    if (n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0)
      return error_EXISTS;

    GET_NEXT_DIR(dir, ukey, ukeyend);
  }

  *pn = dstree__node_create(t, key, value, keylen);
  if (*pn == NULL)
    return error_OOM;

  return error_OK;
}
