/* container-critbit.c -- glue to make a critbit tree be a container */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memento.h"

#include "errors.h"
#include "icontainer.h"
#include "critbit.h"

#include "container-critbit.h"

typedef struct container_critbit
{
  icontainer_t               c;
  critbit_t                 *t;

  icontainer_key_len         len;

  icontainer_kv_show         show_key;
  icontainer_kv_show_destroy show_key_destroy;
  icontainer_kv_show         show_value;
  icontainer_kv_show_destroy show_value_destroy;
}
container_critbit_t;

static const void *container_critbit__lookup(const icontainer_t *c_,
                                             const void         *key)
{
  const container_critbit_t *c = (container_critbit_t *) c_;

  return critbit_lookup(c->t, key, c->len(key));
}

static error container_critbit__insert(icontainer_t *c_,
                                       const void   *key,
                                       const void   *value)
{
  container_critbit_t *c = (container_critbit_t *) c_;

  return critbit_insert(c->t, key, c->len(key), value);
}

static void container_critbit__remove(icontainer_t *c_, const void *key)
{
  container_critbit_t *c = (container_critbit_t *) c_;

  critbit_remove(c->t, key, c->len(key));
}

static const item_t *container_critbit__select(const icontainer_t *c_,
                                               int                 k)
{
  container_critbit_t *c = (container_critbit_t *) c_;

  return critbit_select(c->t, k);
}

static error container_critbit__lookup_prefix(const icontainer_t        *c_,
                                              const void                *prefix,
                                              icontainer_found_callback  cb,
                                              void                      *opaque)
{
  const container_critbit_t *c = (container_critbit_t *) c_;

  /* critbit_lookup_prefix and icontainer_found_callback have the same
   * signature so we can just cast one to the other here. If this were not
   * the case we would need an adaptor function to turn one callback into
   * another. */

  return critbit_lookup_prefix(c->t,
                               prefix, c->len(prefix),
                               (icontainer_found_callback) cb, opaque);
}

static int container_critbit__count(const icontainer_t *c_)
{
  const container_critbit_t *c = (container_critbit_t *) c_;

  return critbit_count(c->t);
}

static error container_critbit__show(const icontainer_t *c_, FILE *f)
{
  container_critbit_t *c = (container_critbit_t *) c_;

  return critbit_show(c->t,
                      c->show_key, c->show_key_destroy,
                      c->show_value, c->show_value_destroy,
                      f);
}

static error container_critbit__show_viz(const icontainer_t *c_, FILE *f)
{
  container_critbit_t *c = (container_critbit_t *) c_;

  return critbit_show_viz(c->t,
                          c->show_key, c->show_key_destroy,
                          c->show_value, c->show_value_destroy,
                          f);
}

static void container_critbit__destroy(icontainer_t *doomed_)
{
  container_critbit_t *doomed = (container_critbit_t *) doomed_;

  critbit_destroy(doomed->t);
  free(doomed);
}

error container_create_critbit(icontainer_t            **container,
                               const icontainer_key_t   *key,
                               const icontainer_value_t *value)
{
  static const icontainer_t methods =
  {
    container_critbit__lookup,
    container_critbit__insert,
    container_critbit__remove,
    container_critbit__select,
    container_critbit__lookup_prefix,
    container_critbit__count,
    container_critbit__show,
    container_critbit__show_viz,
    container_critbit__destroy,
  };

  error                err;
  container_critbit_t *c;

  assert(container);
  assert(key);
  assert(value);

  *container = NULL;

  /* ensure required callbacks are specified */

  if (key->len == NULL)
    return error_KEYLEN_REQUIRED;

  c = malloc(sizeof(*c));
  if (c == NULL)
    return error_OOM;

  c->c                  = methods;

  c->len                = key->len;

  c->show_key           = key->kv.show;
  c->show_key_destroy   = key->kv.show_destroy;
  c->show_value         = value->kv.show;
  c->show_value_destroy = value->kv.show_destroy;

  err = critbit_create(value->default_value,
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
