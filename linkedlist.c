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
                                                   const void   *value)
{
  linkedlist__node_t *n;

  n = malloc(sizeof(*n));
  if (n == NULL)
    return NULL;

  n->next       = NULL;
  n->item.key   = key;
  n->item.value = value;

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

const void *linkedlist_lookup(linkedlist_t *t, const void *key)
{
  linkedlist__node_t *n;

  for (n = t->anchor; n; n = n->next)
    if (t->compare(key, n->item.key) == 0)
      break;

  return n ? n->item.value : t->default_value;
}

/* ----------------------------------------------------------------------- */

error linkedlist_insert(linkedlist_t *t,
                        const void   *key,
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

  n = linkedlist__node_create(t, key, value);
  if (n == NULL)
    return error_OOM;

  n->next = *pn;
  *pn      = n;

  return error_OK;
}

/* ----------------------------------------------------------------------- */

void linkedlist_remove(linkedlist_t *t, const void *key)
{
  linkedlist__node_t **pn;
  linkedlist__node_t  *doomed;

  for (pn = &t->anchor; *pn; pn = &(*pn)->next)
    if (t->compare(key, (*pn)->item.key) == 0)
      break;

  doomed = *pn;
  if (doomed == NULL)
    return; /* not found */

  *pn = doomed->next;

  linkedlist__node_destroy(t, doomed);
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
