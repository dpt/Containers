/* --------------------------------------------------------------------------
 *    Name: insert.c
 * Purpose: Associative array implemented as a PATRICIA tree
 * ----------------------------------------------------------------------- */

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "base/result.h"
#include "base/types.h"
#include "base/utils.h"

#include "utils/utils.h"

#include "datastruct/patricia.h"

#include "impl.h"

result_t patricia_insert(patricia_t *t,
                         const void *key,
                         size_t      keylen,
                         const void *value)
{
  const unsigned char *ukey;
  const unsigned char *ukeyend;
  int                  bit;

  {
    patricia__node_t    *q;
    const unsigned char *qkey;
    const unsigned char *qkeyend;

    q = t->root;
    assert(q != NULL);

    /* keys consisting of all zero bits always live in the root node */
    if (unlikely(iszero(key, keylen)))
      goto update;

    /* find closest node */
    q = (patricia__node_t *) patricia__lookup(q, key, keylen); /* we cast away const */
    assert(q != NULL);

    if (q->item.keylen == keylen && memcmp(q->item.key, key, keylen) == 0)
    {
update:
      if (q->item.key == key)
      {
        /* existing key - just update the value */
        q->item.value  = value;
      }
      else
      {
        patricia__node_clear(t, q);

        q->item.key    = key;
        q->item.keylen = keylen;
        q->item.value  = value;
      }

      return result_OK;
    }

    /* we've found a node which differs */

    qkey    = q->item.key;
    qkeyend = qkey + q->item.keylen;

    ukey    = key;
    ukeyend = ukey + keylen;

    /* locate the critical bit */

    bit = keydiffbit(qkey, qkeyend - qkey, ukey, ukeyend - ukey);
    // do i need the if (bit == -1) test?
  }

  /* insert new item */

  {
    patricia__node_t  *newnode;
    patricia__node_t  *n;
    int                nbit;
    int                parbit;
    patricia__node_t **pn;
    int                newdir;

    /* allocate new node */

    newnode = patricia__node_create(t, key, keylen, value);
    if (newnode == NULL)
      return result_OOM;

    n    = t->root;
    nbit = n->bit;
    do
    {
      parbit = nbit;
      pn     = &n->child[GET_DIR(ukey, ukeyend, nbit)];
      n      = *pn;
      nbit   = n->bit;
    }
    /* while (we've not hit the bit we want && encountered ascending) */
    while (nbit < bit && nbit > parbit);

    /* 'n' now points to a node with a bit index greater than our intended
     * index, or to the head node */

    newnode->bit = bit;

    /* the set bit indexes the direction we take to arrive at ourselves */
    newdir = GET_DIR(ukey, ukeyend, bit);
    assert(newdir == 0 || newdir == 1);

    newnode->child[newdir]  = newnode;
    newnode->child[!newdir] = n;

    *pn = newnode;
  }

  return result_OK;
}

