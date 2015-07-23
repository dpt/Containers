/* --------------------------------------------------------------------------
 *    Name: insert.c
 * Purpose: Associative array implemented as a critbit tree
 * ----------------------------------------------------------------------- */

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "base/result.h"

#include "utils/utils.h"

#include "datastruct/critbit.h"

#include "impl.h"

// Can we insert an all-zero-bits key into an empty critbit tree?
// - Which is the critbit in that case?
// - It turns out that's not a relevant question for crit-bit as the only
// node which needs to be created is an external node.
// ...
// - But for patricia there are no external nodes. /Everything/ must have a
// crit-bit, but you can't discover the crit-bit of an all-zero-bits key
// unless you arbitrarily fix the length of the keys.

result_t critbit_insert(critbit_t  *t,
                        const void *key,
                        size_t      keylen,
                        const void *value)
{
  const unsigned char *ukey;
  const unsigned char *ukeyend;
  int                  newbyte;
  unsigned int         newotherbits;
  const unsigned char *qkey;
  const unsigned char *qkeyend;

  {
    critbit__extnode_t *q;

    /* deal with inserting into an empty tree */
    if (t->root == NULL)
    {
      critbit__extnode_t *newextnode;

      newextnode = critbit__extnode_create(t, key, keylen, value);
      if (newextnode == NULL)
        return result_OOM;

      t->root = TO_STORE(newextnode);

      return result_OK;
    }

    /* find closest node */
    q = (critbit__extnode_t *) critbit__lookup(t->root, key, keylen); /* we cast away const */
    assert(q != NULL);

    if (q->item.keylen == keylen && memcmp(q->item.key, key, keylen) == 0)
    {
      /* update the existing key's value */

      if (q->item.key == key)
      {
        /* existing key - just update the value */
        q->item.value  = value;
      }
      else
      {
        critbit__extnode_clear(t, q);

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

    {
      int nbit;

      nbit = keydiffbit(qkey, qkeyend - qkey, ukey, ukeyend - ukey);
      if (nbit == -1)
        return result_CLASHES;

      newotherbits = 1 << (7 - (nbit & 0x07));
      newotherbits ^= 255;

      newbyte = nbit >> 3;
    }
  }

  /* insert new item */

  {
    critbit__extnode_t *newextnode;
    critbit__node_t    *newnode;
    critbit__node_t   **pn;
    int                 newdir;

    /* allocate new external node */

    newextnode = critbit__extnode_create(t, key, keylen, value);
    if (newextnode == NULL)
      return result_OOM;

    /* allocate new internal node */

    newnode = critbit__node_create(t, newbyte, (uint8_t) newotherbits);
    if (newnode == NULL)
    {
      critbit__extnode_destroy(t, newextnode);
      return result_OOM;
    }

    /* insert new node */

    pn = &t->root;

    for (;;)
    {
      critbit__node_t *n;

      n = *pn;

      if (IS_EXTERNAL(n)    ||
          n->byte > newbyte ||
          (n->byte == newbyte && n->otherbits > newotherbits))
        break;

      pn = &n->child[GET_DIR(ukey, ukeyend, n->byte, n->otherbits)];
    }

    newdir = GET_DIR(qkey, qkeyend, newbyte, newotherbits);
    assert(newdir == 0 || newdir == 1);

    newnode->child[newdir] = *pn;
    newnode->child[!newdir] = TO_STORE(newextnode);

    *pn = newnode;
  }

  return result_OK;
}

