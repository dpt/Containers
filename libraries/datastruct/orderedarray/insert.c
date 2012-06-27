/* --------------------------------------------------------------------------
 *    Name: insert.c
 * Purpose: Associative array implemented as an ordered array
 * ----------------------------------------------------------------------- */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "base/memento/memento.h"

#include "base/errors.h"

#include "datastruct/orderedarray.h"

#include "impl.h"

/* ensure space for 'need' elements */
static error orderedarray__ensure(orderedarray_t *t, int need)
{
  orderedarray__node_t *array;
  int                   nelems;
  int                   maxelems;

  nelems   = t->nelems;
  maxelems = t->maxelems;

  if (maxelems - nelems >= need)
    return error_OK; /* we have enough space */

  if (maxelems < 8)
    maxelems = 8; /* minimum number of elements */

  while (maxelems - nelems < need)
    maxelems *= 2; /* double until we have enough */

  array = realloc(t->array, maxelems * sizeof(*array));
  if (array == NULL)
    return error_OOM;

  t->array    = array;
  t->maxelems = maxelems;

  return error_OK;
}

error orderedarray_insert(orderedarray_t *t,
                          const void     *key,
                          size_t          keylen,
                          const void     *value)
{
  error                 err;
  orderedarray__node_t *n;
  int                   found;
  size_t                m;

  /* we must call 'ensure' before 'lookup' otherwise 'n' will be invalidated
   * when the block is moved by realloc */

  err = orderedarray__ensure(t, 1);
  if (err)
    return err;

  found = orderedarray__lookup_internal(t, key, &n);
  if (found)
    return error_EXISTS;

  assert(n); /* n is always non-NULL after a call to ensure */

  /* shunt up subsequent entries */

  m = t->nelems - (n - t->array);

  if (m)
    memmove(n + 1, n, m * sizeof(*t->array));

  n->item.key    = key;
  n->item.keylen = keylen;
  n->item.value  = value;

  t->nelems++;

  return error_OK;
}

