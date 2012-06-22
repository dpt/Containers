/* --------------------------------------------------------------------------
 *    Name: show.c
 * Purpose: Ordered array
 * ----------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>

#include "base/memento/memento.h"

#include "base/types.h"

#include "datastruct/orderedarray.h"

#include "impl.h"

typedef struct orderedarray__show_args
{
  orderedarray_show_key     *key;
  orderedarray_show_destroy *key_destroy;
  orderedarray_show_value   *value;
  orderedarray_show_destroy *value_destroy;
  FILE                      *f;
}
orderedarray__show_args_t;

static error orderedarray__node_show(orderedarray__node_t *n, void *opaque)
{
  orderedarray__show_args_t *args = opaque;
  const char                *key;
  const char                *value;

  key   = args->key   && n->item.key   ? args->key(n->item.key)     : NULL;
  value = args->value && n->item.value ? args->value(n->item.value) : NULL;

  (void) fprintf(args->f, "orderedarray: %p: %s -> %s\n", n,
                 key   ? key   : "(null)",
                 value ? value : "(null)");

  if (args->key_destroy   && key)   args->key_destroy((char *) key);
  if (args->value_destroy && value) args->value_destroy((char *) value);

  return error_OK;
}

error orderedarray_show(const orderedarray_t      *t,
                        orderedarray_show_key     *key,
                        orderedarray_show_destroy *key_destroy,
                        orderedarray_show_value   *value,
                        orderedarray_show_destroy *value_destroy,
                        FILE                      *f)
{
  orderedarray__show_args_t args;

  args.key           = key;
  args.key_destroy   = key_destroy;
  args.value         = value;
  args.value_destroy = value_destroy;
  args.f             = f;

  return orderedarray__walk_internal((orderedarray_t *) t,
                                     orderedarray__node_show,
                                     &args);
}

