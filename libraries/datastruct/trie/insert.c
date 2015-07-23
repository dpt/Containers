/* --------------------------------------------------------------------------
 *    Name: insert.c
 * Purpose: Associative array implemented as a trie
 * ----------------------------------------------------------------------- */

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "base/result.h"

#include "datastruct/trie.h"

#include "impl.h"

static trie__node_t *trie__insert_split(trie_t       *t,
                                        trie__node_t *m,
                                        trie__node_t *n,
                                        int           depth)
{
  trie__node_t        *x;
  const unsigned char *ukeym;
  const unsigned char *ukeymend;
  const unsigned char *ukeyn;
  const unsigned char *ukeynend;
  int                  mdir;
  int                  ndir;

  x = trie__node_create(t, NULL, 0L, NULL);
  if (x == NULL)
    return NULL; /* OOM */

  ukeym    = m->item.key;
  ukeymend = ukeym + m->item.keylen;

  mdir = 0;
  if (ukeym + (depth >> 3) < ukeymend)
    mdir = (ukeym[depth >> 3] >> (7 - (depth & 7))) & 1;

  ukeyn    = n->item.key;
  ukeynend = ukeyn + n->item.keylen;

  ndir = 0;
  if (ukeyn + (depth >> 3) < ukeynend)
    ndir = (ukeyn[depth >> 3] >> (7 - (depth & 7))) & 1;

  switch (mdir * 2 + ndir)
  {
  case 0:
    x->child[0] = trie__insert_split(t, m, n, depth + 1);
    break;
  case 3:
    x->child[1] = trie__insert_split(t, m, n, depth + 1);
    break;
  case 1:
    x->child[0] = m;
    x->child[1] = n;
    break;
  case 2:
    x->child[0] = n;
    x->child[1] = m;
    break;
  }

  // missing error handling when we return NULL to ourselves

  return x;
}

result_t trie_insert(trie_t     *t,
                     const void *key,
                     size_t      keylen,
                     const void *value)
{
  const unsigned char *ukey    = key;
  const unsigned char *ukeyend = ukey + keylen;
  int                  depth;
  trie__node_t       **pn;
  trie__node_t        *n;
  int                  dir;
  unsigned char        c = 0;
  trie__node_t        *m;

  /* search, but save the parent pointer too */

  depth = 0;

  for (pn = &t->root; (n = *pn); pn = &n->child[dir])
  {
    if (IS_LEAF(n))
      break;

    GET_NEXT_DIR(dir, ukey, ukeyend);
  }

  if (n && n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0)
    return result_EXISTS;

  m = trie__node_create(t, key, keylen, value);
  if (m == NULL)
    return result_OOM;

  if (n)
  {
    /* leaf node: need to split */

    trie__node_t *m2 = m;

    assert(IS_LEAF(n));

    m = trie__insert_split(t, m, n, depth);
    if (m == NULL)
    {
      trie__node_destroy(t, m2);
      return result_OOM;
    }
  }

  *pn = m;

  return result_OK;
}
