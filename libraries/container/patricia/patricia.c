/* --------------------------------------------------------------------------
 *    Name: patricia.c
 * Purpose: Glue to make a PATRICIA trie be a container
 * ----------------------------------------------------------------------- */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "base/memento/memento.h"
#include "base/result.h"
#include "datastruct/patricia.h"
#include "container/interface/container.h"

#include "container/patricia.h"

typedef struct container_patricia
{
  icontainer_t               c;
  patricia_t                *t;

  icontainer_key_len         len;

  icontainer_kv_show         show_key;
  icontainer_kv_show_destroy show_key_destroy;
  icontainer_kv_show         show_value;
  icontainer_kv_show_destroy show_value_destroy;
}
container_patricia_t;

static const void *container_patricia__lookup(const icontainer_t *c_,
                                              const void         *key)
{
  const container_patricia_t *c = (container_patricia_t *) c_;

  return patricia_lookup(c->t, key, c->len(key));
}

static result_t container_patricia__insert(icontainer_t *c_,
                                           const void   *key,
                                           const void   *value)
{
  container_patricia_t *c = (container_patricia_t *) c_;

  return patricia_insert(c->t, key, c->len(key), value);
}

static void container_patricia__remove(icontainer_t *c_, const void *key)
{
  container_patricia_t *c = (container_patricia_t *) c_;

  patricia_remove(c->t, key, c->len(key));
}

static const item_t *container_patricia__select(const icontainer_t *c_,
                                                int                 k)
{
  container_patricia_t *c = (container_patricia_t *) c_;

  return patricia_select(c->t, k);
}

static result_t container_patricia__lookup_prefix(const icontainer_t        *c_,
                                                  const void                *prefix,
                                                  icontainer_found_callback  cb,
                                                  void                      *opaque)
{
  const container_patricia_t *c = (container_patricia_t *) c_;

  /* patricia_lookup_prefix and icontainer_found_callback have the same
   * signature so we can just cast one to the other here. If this were not
   * the case we would need an adaptor function to turn one callback into
   * another. */

  return patricia_lookup_prefix(c->t,
                                prefix, c->len(prefix),
                                (icontainer_found_callback) cb, opaque);
}

static int container_patricia__count(const icontainer_t *c_)
{
  const container_patricia_t *c = (container_patricia_t *) c_;

  return patricia_count(c->t);
}

static result_t container_patricia__show(const icontainer_t *c_, FILE *f)
{
  container_patricia_t *c = (container_patricia_t *) c_;

  return patricia_show(c->t,
                       c->show_key, c->show_key_destroy,
                       c->show_value, c->show_value_destroy,
                       f);
}

static result_t container_patricia__show_viz(const icontainer_t *c_, FILE *f)
{
  container_patricia_t *c = (container_patricia_t *) c_;

  return patricia_show_viz(c->t,
                           c->show_key, c->show_key_destroy,
                           c->show_value, c->show_value_destroy,
                           f);
}

static void container_patricia__destroy(icontainer_t *doomed_)
{
  container_patricia_t *doomed = (container_patricia_t *) doomed_;

  patricia_destroy(doomed->t);
  free(doomed);
}

result_t container_create_patricia(icontainer_t            **container,
                                   const icontainer_key_t   *key,
                                   const icontainer_value_t *value)
{
  static const icontainer_t methods =
  {
    container_patricia__lookup,
    container_patricia__insert,
    container_patricia__remove,
    container_patricia__select,
    container_patricia__lookup_prefix,
    container_patricia__count,
    container_patricia__show,
    container_patricia__show_viz,
    container_patricia__destroy,
  };

  result_t              err;
  container_patricia_t *c;

  assert(container);
  assert(key);
  assert(value);

  *container = NULL;

  /* ensure required callbacks are specified */

  if (key->len == NULL)
    return result_KEYLEN_REQUIRED;

  c = malloc(sizeof(*c));
  if (c == NULL)
    return result_OOM;

  c->c                  = methods;

  c->len                = key->len;

  c->show_key           = key->kv.show;
  c->show_key_destroy   = key->kv.show_destroy;
  c->show_value         = value->kv.show;
  c->show_value_destroy = value->kv.show_destroy;

  err = patricia_create(value->default_value,
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
