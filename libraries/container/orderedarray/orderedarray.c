/* orderedarray.c -- glue to make an orderedarray be a container */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "base/memento/memento.h"
#include "base/errors.h"
#include "base/types.h"
#include "datastruct/orderedarray.h"
#include "container/interface/container.h"

#include "container/orderedarray.h"

typedef struct container_orderedarray
{
  icontainer_t               c;
  orderedarray_t            *t;

  icontainer_key_len         len;

  icontainer_kv_show         show_key;
  icontainer_kv_show_destroy show_key_destroy;
  icontainer_kv_show         show_value;
  icontainer_kv_show_destroy show_value_destroy;
}
container_orderedarray_t;

static const void *container_orderedarray__lookup(const icontainer_t *c_,
                                                  const void         *key)
{
  const container_orderedarray_t *c = (container_orderedarray_t *) c_;

  return orderedarray_lookup(c->t, key);
}

static error container_orderedarray__insert(icontainer_t *c_,
                                            const void   *key,
                                            const void   *value)
{
  container_orderedarray_t *c = (container_orderedarray_t *) c_;

  return orderedarray_insert(c->t, key, c->len(key), value);
}

static void container_orderedarray__remove(icontainer_t *c_, const void *key)
{
  container_orderedarray_t *c = (container_orderedarray_t *) c_;

  orderedarray_remove(c->t, key);
}

static const item_t *container_orderedarray__select(const icontainer_t *c_,
                                                    int                 k)
{
  container_orderedarray_t *c = (container_orderedarray_t *) c_;

  return orderedarray_select(c->t, k);
}

static error container_orderedarray__lookup_prefix(const icontainer_t        *c_,
                                                   const void                *prefix,
                                                   icontainer_found_callback  cb,
                                                   void                      *opaque)
{
  const container_orderedarray_t *c = (container_orderedarray_t *) c_;

  /* orderedarray_lookup_prefix_callback and icontainer_found_callback have
   * the same signature so we can just cast one to the other here. If this
   * were not the case we would need an adaptor function to turn one callback
   * into another. */

  return orderedarray_lookup_prefix(c->t,
                                    prefix, c->len(prefix),
                                    (icontainer_found_callback) cb, opaque);
}

static int container_orderedarray__count(const icontainer_t *c_)
{
  container_orderedarray_t *c = (container_orderedarray_t *) c_;

  return orderedarray_count(c->t);
}

static error container_orderedarray__show(const icontainer_t *c_, FILE *f)
{
  container_orderedarray_t *c = (container_orderedarray_t *) c_;

  return orderedarray_show(c->t,
                           c->show_key, c->show_key_destroy,
                           c->show_value, c->show_value_destroy,
                           f);
}

static error container_orderedarray__show_viz(const icontainer_t *c_, FILE *f)
{
  NOT_USED(c_);
  NOT_USED(f);

  return error_OK; // NYI
}

static void container_orderedarray__destroy(icontainer_t *doomed_)
{
  container_orderedarray_t *doomed = (container_orderedarray_t *) doomed_;

  orderedarray_destroy(doomed->t);
  free(doomed);
}

error container_create_orderedarray(icontainer_t            **container,
                                    const icontainer_key_t   *key,
                                    const icontainer_value_t *value)
{
  static const icontainer_t methods =
  {
    container_orderedarray__lookup,
    container_orderedarray__insert,
    container_orderedarray__remove,
    container_orderedarray__select,
    container_orderedarray__lookup_prefix,
    container_orderedarray__count,
    container_orderedarray__show,
    container_orderedarray__show_viz,
    container_orderedarray__destroy,
  };

  error                     err;
  container_orderedarray_t *c;

  assert(container);
  assert(key);
  assert(value);

  *container = NULL;

  /* ensure required callbacks are specified */

  if (key->len == NULL)
    return error_KEYLEN_REQUIRED;
  if (key->compare == NULL)
    return error_KEYCOMPARE_REQUIRED;

  c = malloc(sizeof(*c));
  if (c == NULL)
    return error_OOM;

  c->c                  = methods;

  c->len                = key->len;

  c->show_key           = key->kv.show;
  c->show_key_destroy   = key->kv.show_destroy;
  c->show_value         = value->kv.show;
  c->show_value_destroy = value->kv.show_destroy;

  err = orderedarray_create(value->default_value,
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

  return error_OK;
}
