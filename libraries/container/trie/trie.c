/* trie.c -- glue to make a trie be a container */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "base/memento/memento.h"
#include "base/errors.h"
#include "datastruct/trie.h"
#include "container/interface/container.h"

#include "container/trie.h"

typedef struct container_trie
{
  icontainer_t               c;
  trie_t                    *t;

  icontainer_key_len         len;

  icontainer_kv_show         show_key;
  icontainer_kv_show_destroy show_key_destroy;
  icontainer_kv_show         show_value;
  icontainer_kv_show_destroy show_value_destroy;
}
container_trie_t;

static const void *container_trie__lookup(const icontainer_t *c_,
                                          const void         *key)
{
  const container_trie_t *c = (container_trie_t *) c_;

  return trie_lookup(c->t, key, c->len(key));
}

static error container_trie__insert(icontainer_t *c_,
                                    const void   *key,
                                    const void   *value)
{
  container_trie_t *c = (container_trie_t *) c_;

  return trie_insert(c->t, key, c->len(key), value);
}

static void container_trie__remove(icontainer_t *c_, const void *key)
{
  container_trie_t *c = (container_trie_t *) c_;

  trie_remove(c->t, key, c->len(key));
}

static const item_t *container_trie__select(const icontainer_t *c_, int k)
{
  container_trie_t *c = (container_trie_t *) c_;

  return trie_select(c->t, k);
}

static error container_trie__lookup_prefix(const icontainer_t        *c_,
                                           const void                *prefix,
                                           icontainer_found_callback  cb,
                                           void                      *opaque)
{
  const container_trie_t *c = (container_trie_t *) c_;

  /* trie_lookup_prefix_callback and icontainer_found_callback have the
   * same signature so we can just cast one to the other here. If this were
   * not the case we would need an adaptor function to turn one callback into
   * another. */

  return trie_lookup_prefix(c->t,
                            prefix, c->len(prefix),
                            (icontainer_found_callback) cb, opaque);
}

static int container_trie__count(const icontainer_t *c_)
{
  const container_trie_t *c = (container_trie_t *) c_;

  return trie_count(c->t);
}

static error container_trie__show(const icontainer_t *c_, FILE *f)
{
  container_trie_t *c = (container_trie_t *) c_;

  return trie_show(c->t,
                   c->show_key, c->show_key_destroy,
                   c->show_value, c->show_value_destroy,
                   f);
}

static error container_trie__show_viz(const icontainer_t *c_, FILE *f)
{
  container_trie_t *c = (container_trie_t *) c_;

  return trie_show_viz(c->t,
                       c->show_key, c->show_key_destroy,
                       c->show_value, c->show_value_destroy,
                       f);
}

static void container_trie__destroy(icontainer_t *doomed_)
{
  container_trie_t *doomed = (container_trie_t *) doomed_;

  trie_destroy(doomed->t);
  free(doomed);
}

error container_create_trie(icontainer_t            **container,
                            const icontainer_key_t   *key,
                            const icontainer_value_t *value)
{
  static const icontainer_t methods =
  {
    container_trie__lookup,
    container_trie__insert,
    container_trie__remove,
    container_trie__select,
    container_trie__lookup_prefix,
    container_trie__count,
    container_trie__show,
    container_trie__show_viz,
    container_trie__destroy,
  };

  error             err;
  container_trie_t *c;

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

  err = trie_create(value->default_value,
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
