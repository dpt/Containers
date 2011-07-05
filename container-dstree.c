/* container-dstree.c -- glue to make a dstree be a container */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memento.h"

#include "errors.h"
#include "icontainer.h"
#include "dstree.h"

#include "container-dstree.h"

typedef struct container_dstree
{
  icontainer_t               c;
  dstree_t                  *t;

  icontainer_key_len         len;

  icontainer_kv_show         show_key;
  icontainer_kv_show_destroy show_key_destroy;
  icontainer_kv_show         show_value;
  icontainer_kv_show_destroy show_value_destroy;
}
container_dstree_t;

static const void *container_dstree__lookup(const icontainer_t *c_,
                                            const void         *key)
{
  const container_dstree_t *c = (container_dstree_t *) c_;

  return dstree_lookup(c->t, key, c->len(key));
}

static error container_dstree__insert(icontainer_t *c_,
                                      const void   *key,
                                      const void   *value)
{
  container_dstree_t *c = (container_dstree_t *) c_;

  return dstree_insert(c->t, key, c->len(key), value);
}

static void container_dstree__remove(icontainer_t *c_, const void *key)
{
  container_dstree_t *c = (container_dstree_t *) c_;

  dstree_remove(c->t, key, c->len(key));
}

static const item_t *container_dstree__select(const icontainer_t *c_, int k)
{
  NOT_USED(c_);
  NOT_USED(k);

  return NULL; /* not implemented */
}

static int container_dstree__count(const icontainer_t *c_)
{
  const container_dstree_t *c = (container_dstree_t *) c_;

  return dstree_count(c->t);
}

static error container_dstree__show(const icontainer_t *c_, FILE *f)
{
  container_dstree_t *c = (container_dstree_t *) c_;

  return dstree_show(c->t,
                     c->show_key, c->show_key_destroy,
                     c->show_value, c->show_value_destroy,
                     f);
}

static error container_dstree__show_viz(const icontainer_t *c_, FILE *f)
{
  NOT_USED(c_);
  NOT_USED(f);

  return error_OK; /* not implemented */
}

static void container_dstree__destroy(icontainer_t *doomed_)
{
  container_dstree_t *doomed = (container_dstree_t *) doomed_;

  dstree_destroy(doomed->t);
  free(doomed);
}

error container_create_dstree(icontainer_t            **container,
                              const icontainer_key_t   *key,
                              const icontainer_value_t *value)
{
  static const icontainer_t methods =
  {
    container_dstree__lookup,
    container_dstree__insert,
    container_dstree__remove,
    container_dstree__select,
    container_dstree__count,
    container_dstree__show,
    container_dstree__show_viz,
    container_dstree__destroy,
  };

  error               err;
  container_dstree_t *c;

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

  err = dstree_create(value->default_value,
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
