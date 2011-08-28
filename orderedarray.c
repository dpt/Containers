/* orderedarray.c -- associative array implemented as ordered array */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memento.h"

#include "types.h"
#include "string.h"
#include "string-kv.h"

#include "orderedarray.h"

#include "orderedarray-impl.h"

/* ----------------------------------------------------------------------- */

static void orderedarray__node_destroy(orderedarray_t       *t,
                                       orderedarray__node_t *n)
{
  size_t m;

  if (t->destroy_key)
    t->destroy_key((void *) n->item.key); /* must cast away const */
  if (t->destroy_value)
    t->destroy_value((void *) n->item.value);

  m = t->nelems - (n - t->array + 1);

  if (m)
    memmove(n, n + 1, m * sizeof(*t->array));

  t->nelems--;
}

/* ----------------------------------------------------------------------- */

/* NULL passed into the compare/destroy callbacks signifies that these
 * internal routines should be used. They are set up to handle malloc'd
 * strings.
 */

error orderedarray_create(const void                  *default_value,
                          orderedarray_compare        *compare,
                          orderedarray_destroy_key    *destroy_key,
                          orderedarray_destroy_value  *destroy_value,
                          orderedarray_t             **pt)
{
  orderedarray_t *t;

  *pt = NULL;

  t = malloc(sizeof(*t));
  if (t == NULL)
    return error_OOM;

  t->array         = NULL;
  t->nelems        = 0;
  t->maxelems      = 0;

  t->default_value = default_value;
  t->compare       = compare       ? compare       : stringkv_compare;
  t->destroy_key   = destroy_key   ? destroy_key   : stringkv_destroy;
  t->destroy_value = destroy_value ? destroy_value : stringkv_destroy;

  *pt = t;

  return error_OK;
}

static error orderedarray__destroy_node(orderedarray__node_t *n,
                                        void                 *opaque)
{
  orderedarray__node_destroy(opaque, n);

  return error_OK;
}

void orderedarray_destroy(orderedarray_t *t)
{
  (void) orderedarray__walk_internal(t, orderedarray__destroy_node, t);

  free(t->array);

  free(t);
}

/* ----------------------------------------------------------------------- */

/* returns: found/not found */
static const int orderedarray__lookup(orderedarray_t        *t,
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

  return orderedarray__lookup(t, key, &n) ? n->item.value : t->default_value;
}

/* ----------------------------------------------------------------------- */

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

  found = orderedarray__lookup(t, key, &n);
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

/* ----------------------------------------------------------------------- */

void orderedarray_remove(orderedarray_t *t, const void *key)
{
  orderedarray__node_t *n;

  if (!orderedarray__lookup(t, key, &n))
    return; /* not found */

  orderedarray__node_destroy(t, n);
}

/* ----------------------------------------------------------------------- */

const item_t *orderedarray_select(orderedarray_t *t, int k)
{
  return (k < t->nelems) ? &t->array[k].item : t->default_value;
}

/* ----------------------------------------------------------------------- */

int orderedarray_count(orderedarray_t *t)
{
  return t->nelems;
}

/* ----------------------------------------------------------------------- */

typedef struct orderedarray_lookup_prefix_args
{
  const unsigned char         *uprefix;
  size_t                       prefixlen;
  orderedarray_found_callback *cb;
  void                        *opaque;
}
orderedarray_lookup_prefix_args_t;

static error orderedarray__lookup_prefix(orderedarray__node_t *n,
                                         void                 *opaque)
{
  const orderedarray_lookup_prefix_args_t *args = opaque;
  size_t                                   prefixlen;
  
  prefixlen = args->prefixlen;
  
  if (n->item.keylen >= prefixlen &&
      memcmp(n->item.key, args->uprefix, prefixlen) == 0)
  {
    return args->cb(&n->item, args->opaque);
  }
  else
  {
    return error_OK;
  }
}

error orderedarray_lookup_prefix(const orderedarray_t        *t,
                                 const void                  *prefix,
                                 size_t                       prefixlen,
                                 orderedarray_found_callback *cb,
                                 void                        *opaque)
{
  orderedarray_lookup_prefix_args_t args;
  
  args.uprefix   = prefix;
  args.prefixlen = prefixlen;
  args.cb        = cb;
  args.opaque    = opaque;
  
  return orderedarray__walk_internal((orderedarray_t *) t, // must cast away constness
                                     orderedarray__lookup_prefix,
                                     &args);
}

/* ----------------------------------------------------------------------- */

error orderedarray__walk_internal(orderedarray_t                       *t,
                                  orderedarray__walk_internal_callback *cb,
                                  void                                 *opaque)
{
  error                 err;
  orderedarray__node_t *n;

  if (t == NULL)
    return error_OK;

  /* don't pre-calculate the end position as it needs to be evaluated on
   * every step: the callback is permitted to delete the current element */

  for (n = t->array; n < t->array + t->nelems; n++)
  {
    err = cb(n, opaque);
    if (err)
      return err;
  }

  return error_OK;
}

/* ----------------------------------------------------------------------- */
