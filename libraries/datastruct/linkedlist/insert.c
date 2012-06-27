/* --------------------------------------------------------------------------
 *    Name: insert.c
 * Purpose: Associative array implemented as a linked list
 * ----------------------------------------------------------------------- */

#include "base/errors.h"

#include "datastruct/linkedlist.h"

#include "impl.h"

error linkedlist_insert(linkedlist_t *t,
                        const void   *key,
                        size_t        keylen,
                        const void   *value)
{
  linkedlist__node_t **pn;
  linkedlist__node_t  *n;
  int                  c;

  /* locate an element to go in front */
  for (pn = &t->anchor; *pn; pn = &(*pn)->next)
    if ((c = t->compare(key, (*pn)->item.key)) <= 0)
      break;

  if (*pn && c == 0)
    return error_EXISTS;

  n = linkedlist__node_create(t, key, keylen, value);
  if (n == NULL)
    return error_OOM;

  n->next = *pn;
  *pn     = n;

  return error_OK;
}

