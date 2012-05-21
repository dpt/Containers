/* container-hash.c -- glue to make a hash be a container */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "memento.h"

#include "errors.h"
#include "utils.h"
#include "icontainer.h"
#include "hash.h"

#include "container-hash.h"

typedef struct container_hash
{
  icontainer_t               c;
  hash_t                    *t;

  icontainer_key_len         len;

  icontainer_kv_show         show_key;
  icontainer_kv_show_destroy show_key_destroy;
  icontainer_kv_show         show_value;
  icontainer_kv_show_destroy show_value_destroy;
}
container_hash_t;

static const void *container_hash__lookup(const icontainer_t *c_,
                                          const void         *key)
{
  const container_hash_t *c = (container_hash_t *) c_;

  return hash_lookup(c->t, key);
}

static error container_hash__insert(icontainer_t *c_,
                                    const void   *key,
                                    const void   *value)
{
  container_hash_t *c = (container_hash_t *) c_;

  return hash_insert(c->t, key, c->len(key), value);
}

static void container_hash__remove(icontainer_t *c_, const void *key)
{
  container_hash_t *c = (container_hash_t *) c_;

  hash_remove(c->t, key);
}

static const item_t *container_hash__select(const icontainer_t *c_, int k)
{
  NOT_USED(c_);
  NOT_USED(k);

  return NULL; /* not implemented */
}

static error container_hash__lookup_prefix(const icontainer_t        *c_,
                                           const void                *prefix,
                                           icontainer_found_callback  cb,
                                           void                      *opaque)
{
  return error_NOT_IMPLEMENTED;
}

static int container_hash__count(const icontainer_t *c_)
{
  const container_hash_t *c = (container_hash_t *) c_;

  return hash_count(c->t);
}

static error container_hash__show(const icontainer_t *c_, FILE *f)
{
  container_hash_t *c = (container_hash_t *) c_;

  return hash_show(c->t,
                   c->show_key, c->show_key_destroy,
                   c->show_value, c->show_value_destroy,
                   f);
}

static error container_hash__show_viz(const icontainer_t *c_, FILE *f)
{
  return error_NOT_IMPLEMENTED;
}

static void container_hash__destroy(icontainer_t *doomed_)
{
  container_hash_t *doomed = (container_hash_t *) doomed_;

  hash_destroy(doomed->t);
  free(doomed);
}

error container_create_hash(icontainer_t            **container,
                            const icontainer_key_t   *key,
                            const icontainer_value_t *value)
{
  static const icontainer_t methods =
  {
    container_hash__lookup,
    container_hash__insert,
    container_hash__remove,
    container_hash__select,
    container_hash__lookup_prefix,
    container_hash__count,
    container_hash__show,
    container_hash__show_viz,
    container_hash__destroy,
  };

  error             err;
  container_hash_t *c;

  assert(container);
  assert(key);
  assert(value);

  *container = NULL;

  /* ensure required callbacks are specified */

  if (key->len == NULL)
    return error_KEYLEN_REQUIRED;
  if (key->compare == NULL)
    return error_KEYCOMPARE_REQUIRED;
  if (key->hash == NULL)
    return error_KEYHASH_REQIURED;

  c = malloc(sizeof(*c));
  if (c == NULL)
    return error_OOM;

  c->c                  = methods;

  c->len                = key->len;

  c->show_key           = key->kv.show;
  c->show_key_destroy   = key->kv.show_destroy;
  c->show_value         = value->kv.show;
  c->show_value_destroy = value->kv.show_destroy;

  err = hash_create(value->default_value,
                    97,
                    key->hash,
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
