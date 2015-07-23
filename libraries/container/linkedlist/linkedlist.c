/* --------------------------------------------------------------------------
 *    Name: linkedlist.c
 * Purpose: Glue to make a linked list be a container
 * ----------------------------------------------------------------------- */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "base/memento/memento.h"
#include "base/result.h"
#include "base/types.h"
#include "container/interface/container.h"
#include "datastruct/linkedlist.h"

#include "container/linkedlist.h"

typedef struct container_linkedlist
{
  icontainer_t               c;
  linkedlist_t              *t;

  icontainer_key_len         len;

  icontainer_kv_show         show_key;
  icontainer_kv_show_destroy show_key_destroy;
  icontainer_kv_show         show_value;
  icontainer_kv_show_destroy show_value_destroy;
}
container_linkedlist_t;

static const void *container_linkedlist__lookup(const icontainer_t *c_,
                                                const void         *key)
{
  const container_linkedlist_t *c = (container_linkedlist_t *) c_;

  return linkedlist_lookup(c->t, key, c->len(key));
}

static result_t container_linkedlist__insert(icontainer_t *c_,
                                             const void   *key,
                                             const void   *value)
{
  container_linkedlist_t *c = (container_linkedlist_t *) c_;

  return linkedlist_insert(c->t, key, c->len(key), value);
}

static void container_linkedlist__remove(icontainer_t *c_, const void *key)
{
  container_linkedlist_t *c = (container_linkedlist_t *) c_;

  linkedlist_remove(c->t, key, c->len(key));
}

static const item_t *container_linkedlist__select(const icontainer_t *c_,
                                                  int                 k)
{
  container_linkedlist_t *c = (container_linkedlist_t *) c_;

  return linkedlist_select(c->t, k);
}

static result_t container_linkedlist__lookup_prefix(const icontainer_t        *c_,
                                                    const void                *prefix,
                                                    icontainer_found_callback  cb,
                                                    void                      *opaque)
{
  const container_linkedlist_t *c = (container_linkedlist_t *) c_;

  /* linkedlist_lookup_prefix_callback and icontainer_found_callback have the
   * same signature so we can just cast one to the other here. If this were
   * not the case we would need an adaptor function to turn one callback into
   * another. */

  return linkedlist_lookup_prefix(c->t,
                                  prefix, c->len(prefix),
                                  (icontainer_found_callback) cb, opaque);
}

static int container_linkedlist__count(const icontainer_t *c_)
{
  container_linkedlist_t *c = (container_linkedlist_t *) c_;

  return linkedlist_count(c->t);
}

static result_t container_linkedlist__show(const icontainer_t *c_, FILE *f)
{
  container_linkedlist_t *c = (container_linkedlist_t *) c_;

  return linkedlist_show(c->t,
                         c->show_key, c->show_key_destroy,
                         c->show_value, c->show_value_destroy,
                         f);
}

static result_t container_linkedlist__show_viz(const icontainer_t *c_, FILE *f)
{
  NOT_USED(c_);
  NOT_USED(f);

  return result_OK; // NYI
}

static void container_linkedlist__destroy(icontainer_t *doomed_)
{
  container_linkedlist_t *doomed = (container_linkedlist_t *) doomed_;

  linkedlist_destroy(doomed->t);
  free(doomed);
}

result_t container_create_linkedlist(icontainer_t            **container,
                                     const icontainer_key_t   *key,
                                     const icontainer_value_t *value)
{
  static const icontainer_t methods =
  {
    container_linkedlist__lookup,
    container_linkedlist__insert,
    container_linkedlist__remove,
    container_linkedlist__select,
    container_linkedlist__lookup_prefix,
    container_linkedlist__count,
    container_linkedlist__show,
    container_linkedlist__show_viz,
    container_linkedlist__destroy,
  };

  result_t                err;
  container_linkedlist_t *c;

  assert(container);
  assert(key);
  assert(value);

  *container = NULL;

  /* ensure required callbacks are specified */

  if (key->len == NULL)
    return result_KEYLEN_REQUIRED;
  if (key->compare == NULL)
    return result_KEYCOMPARE_REQUIRED;

  c = malloc(sizeof(*c));
  if (c == NULL)
    return result_OOM;

  c->c                  = methods;

  c->len                = key->len;

  c->show_key           = key->kv.show;
  c->show_key_destroy   = key->kv.show_destroy;
  c->show_value         = value->kv.show;
  c->show_value_destroy = value->kv.show_destroy;

  err = linkedlist_create(value->default_value,
                          key->compare,
                          key->kv.destroy,
                          value->kv.destroy,
                          &c->t);
  if (err)
  {
    free(c);
    return err;
  }

  *container = &c->c;

  return result_OK;
}
