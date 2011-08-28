/* linkedlist.c -- associative array implemented as ordered linked list */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memento.h"

#include "types.h"
#include "string.h"
#include "string-kv.h"

#include "linkedlist.h"

#include "linkedlist-impl.h"

/* ----------------------------------------------------------------------- */

static linkedlist__node_t *linkedlist__node_create(linkedlist_t *t,
                                                   const void   *key,
                                                   size_t        keylen,
                                                   const void   *value)
{
  linkedlist__node_t *n;

  n = malloc(sizeof(*n));
  if (n == NULL)
    return NULL;

  n->next        = NULL;
  n->item.key    = key;
  n->item.keylen = keylen;
  n->item.value  = value;

  t->count++;

  return n;
}

static void linkedlist__node_destroy(linkedlist_t       *t,
                                     linkedlist__node_t *n)
{
  if (t->destroy_key)
    t->destroy_key((void *) n->item.key); /* must cast away const */
  if (t->destroy_value)
    t->destroy_value((void *) n->item.value);

  free(n);

  t->count--;
}

/* ----------------------------------------------------------------------- */

/* NULL passed into the compare/destroy callbacks signifies that these
 * internal routines should be used. They are set up to handle malloc'd
 * strings.
 */

error linkedlist_create(const void                *default_value,
                        linkedlist_compare        *compare,
                        linkedlist_destroy_key    *destroy_key,
                        linkedlist_destroy_value  *destroy_value,
                        linkedlist_t             **pt)
{
  linkedlist_t *t;

  *pt = NULL;

  t = malloc(sizeof(*t));
  if (t == NULL)
    return error_OOM;

  t->anchor        = NULL;

  t->default_value = default_value;
  t->compare       = compare       ? compare       : stringkv_compare;
  t->destroy_key   = destroy_key   ? destroy_key   : stringkv_destroy;
  t->destroy_value = destroy_value ? destroy_value : stringkv_destroy;

  t->count         = 0;

  *pt = t;

  return error_OK;
}

static error linkedlist__destroy_node(linkedlist__node_t *n,
                                      void               *opaque)
{
  linkedlist__node_destroy(opaque, n);

  return error_OK;
}

void linkedlist_destroy(linkedlist_t *t)
{
  (void) linkedlist__walk_internal(t, linkedlist__destroy_node, t);

  free(t);
}

/* ----------------------------------------------------------------------- */

const void *linkedlist_lookup(linkedlist_t *t,
                              const void   *key,
                              size_t        keylen)
{
  linkedlist__node_t *n;

  for (n = t->anchor; n; n = n->next)
    if (n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0)
      break;

  return n ? n->item.value : t->default_value;
}

/* ----------------------------------------------------------------------- */

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
  *pn      = n;

  return error_OK;
}

/* ----------------------------------------------------------------------- */

void linkedlist_remove(linkedlist_t *t, const void *key, size_t keylen)
{
  linkedlist__node_t **pn;
  linkedlist__node_t  *n;

  for (pn = &t->anchor; (n = *pn); pn = &(*pn)->next)
    if (n->item.keylen == keylen && memcmp(n->item.key, key, keylen) == 0)
      break;

  if (n == NULL)
    return; /* not found */

  *pn = n->next;

  linkedlist__node_destroy(t, n);
}

/* ----------------------------------------------------------------------- */

const item_t *linkedlist_select(linkedlist_t *t, int k)
{
  linkedlist__node_t *n;

  for (n = t->anchor; n; n = n->next)
    if (k-- == 0)
      break;

  return n ? &n->item : NULL;
}

/* ----------------------------------------------------------------------- */

int linkedlist_count(linkedlist_t *t)
{
  return t->count;
}

/* ----------------------------------------------------------------------- */

typedef struct linkedlist_lookup_prefix_args
{
  const unsigned char       *uprefix;
  size_t                     prefixlen;
  linkedlist_found_callback *cb;
  void                      *opaque;
}
linkedlist_lookup_prefix_args_t;

static error linkedlist__lookup_prefix(linkedlist__node_t *n,
                                       void               *opaque)
{
  const linkedlist_lookup_prefix_args_t *args = opaque;
  size_t                                 prefixlen;
  
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

error linkedlist_lookup_prefix(const linkedlist_t        *t,
                               const void                *prefix,
                               size_t                     prefixlen,
                               linkedlist_found_callback *cb,
                               void                      *opaque)
{
  linkedlist_lookup_prefix_args_t args;
  
  args.uprefix   = prefix;
  args.prefixlen = prefixlen;
  args.cb        = cb;
  args.opaque    = opaque;
  
  return linkedlist__walk_internal((linkedlist_t *) t, // must cast away constness
                                   linkedlist__lookup_prefix,
                                   &args);
}

/* ----------------------------------------------------------------------- */

error linkedlist__walk_internal(linkedlist_t                       *t,
                                linkedlist__walk_internal_callback *cb,
                                void                               *opaque)
{
  error               err;
  linkedlist__node_t *n;
  linkedlist__node_t *next;

  if (t == NULL)
    return error_OK;

  for (n = t->anchor; n; n = next)
  {
    next = n->next;

    err = cb(n, opaque);
    if (err)
      return err;
  }

  return error_OK;
}

/* ----------------------------------------------------------------------- */
