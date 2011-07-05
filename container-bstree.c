/* container-bstree.c -- glue to make a bstree be a container */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "memento.h"

#include "errors.h"
#include "icontainer.h"
#include "bstree.h"

#include "container-bstree.h"

typedef struct container_bstree
{
  icontainer_t               c;
  bstree_t                  *t;

  icontainer_kv_show         show_key;
  icontainer_kv_show_destroy show_key_destroy;
  icontainer_kv_show         show_value;
  icontainer_kv_show_destroy show_value_destroy;
}
container_bstree_t;

static const void *container_bstree__lookup(const icontainer_t *c_,
                                            const void         *key)
{
  const container_bstree_t *c = (container_bstree_t *) c_;

  return bstree_lookup(c->t, key);
}

static error container_bstree__insert(icontainer_t *c_,
                                      const void   *key,
                                      const void   *value)
{
  container_bstree_t *c = (container_bstree_t *) c_;

  return bstree_insert(c->t, key, value);
}

static void container_bstree__remove(icontainer_t *c_, const void *key)
{
  container_bstree_t *c = (container_bstree_t *) c_;

  bstree_remove(c->t, key);
}

static const item_t *container_bstree__select(const icontainer_t *c_, int k)
{
  container_bstree_t *c = (container_bstree_t *) c_;

  return bstree_select(c->t, k);
}

static int container_bstree__count(const icontainer_t *c_)
{
  const container_bstree_t *c = (container_bstree_t *) c_;

  return bstree_count(c->t);
}

static error container_bstree__show(const icontainer_t *c_, FILE *f)
{
  container_bstree_t *c = (container_bstree_t *) c_;

  return bstree_show(c->t,
                     c->show_key, c->show_key_destroy,
                     c->show_value, c->show_value_destroy,
                     f);
}

static error container_bstree__show_viz(const icontainer_t *c_, FILE *f)
{
  NOT_USED(c_);
  NOT_USED(f);

  return error_OK; // NYI
}

static void container_bstree__destroy(icontainer_t *doomed_)
{
  container_bstree_t *doomed = (container_bstree_t *) doomed_;

  bstree_destroy(doomed->t);
  free(doomed);
}

error container_create_bstree(icontainer_t            **container,
                              const icontainer_key_t   *key,
                              const icontainer_value_t *value)
{
  static const icontainer_t methods =
  {
    container_bstree__lookup,
    container_bstree__insert,
    container_bstree__remove,
    container_bstree__select,
    container_bstree__count,
    container_bstree__show,
    container_bstree__show_viz,
    container_bstree__destroy,
  };

  error               err;
  container_bstree_t *c;

  assert(container);
  assert(key);
  assert(value);

  *container = NULL;

  /* ensure required callbacks are specified */

  if (key->compare == NULL)
    return error_KEYCOMPARE_REQUIRED;

  c = malloc(sizeof(*c));
  if (c == NULL)
    return error_OOM;

  c->c                  = methods;

  c->show_key           = key->kv.show;
  c->show_key_destroy   = key->kv.show_destroy;
  c->show_value         = value->kv.show;
  c->show_value_destroy = value->kv.show_destroy;

  err = bstree_create(value->default_value,
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
