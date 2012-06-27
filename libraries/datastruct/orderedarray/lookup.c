/* --------------------------------------------------------------------------
 *    Name: lookup.c
 * Purpose: Associative array implemented as an ordered array
 * ----------------------------------------------------------------------- */

#include <string.h>

#include "datastruct/orderedarray.h"

#include "impl.h"

/* returns: found/not found */
int orderedarray__lookup_internal(orderedarray_t        *t,
                                  const void            *key,
                                  orderedarray__node_t **n)
{
  if (t->array)
  {
    orderedarray__node_t *s;
    orderedarray__node_t *e;

    /* binary search */

    s = t->array;
    e = t->array + t->nelems; /* exclusive */

    while (s < e)
    {
      orderedarray__node_t *m;
      int                   r;

      m = s + (e - s) / 2;
      r = t->compare(key, m->item.key);
      if (r == 0)
      {
        *n = m;
        return 1; /* found */
      }
      else if (r < 0)
      {
        e = m;
      }
      else
      {
        s = m + 1;
      }
    }

    *n = s;
  }
  else
  {
    *n = NULL;
  }

  return 0; /* not found */
}

const void *orderedarray_lookup(orderedarray_t *t, const void *key)
{
  orderedarray__node_t *n;

  return orderedarray__lookup_internal(t, key, &n) ? n->item.value :
                                                     t->default_value;
}

