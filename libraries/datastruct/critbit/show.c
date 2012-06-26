/* --------------------------------------------------------------------------
 *    Name: show.c
 * Purpose: Associative array implemented as a critbit tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "base/errors.h"

#include "datastruct/critbit.h"

#include "impl.h"

typedef struct critbit__show_args
{
  critbit_show_key     *key;
  critbit_show_destroy *key_destroy;
  critbit_show_value   *value;
  critbit_show_destroy *value_destroy;
  FILE                 *f;
}
critbit__show_args_t;

static error critbit__node_show(critbit__node_t *n, int level, void *opaque)
{
  static const char stars[] = "********************************"; // works up to 32 levels deep

  critbit__show_args_t *args = opaque;
  const char           *key;
  const char           *value;

  if (IS_INTERNAL(n))
  {
    (void) fprintf(args->f, "critbit: %p: %-32.*s : [l=%p r=%p]\n",
                   n, level + 1, stars,
                   n->child[0], n->child[1]);
  }
  else
  {
    critbit__extnode_t *ext;

    ext = FROM_STORE(n);

    key   = args->key   && ext->item.key   ? args->key(ext->item.key)     : NULL;
    value = args->value && ext->item.value ? args->value(ext->item.value) : NULL;

    (void) fprintf(args->f, "critbit: %p: %-32.*s : %s -> %s\n",
                   ext, level + 1, stars,
                   key   ? key   : "(null)",
                   value ? value : "(null)");

    if (args->key_destroy   && key)   args->key_destroy((char *) key);
    if (args->value_destroy && value) args->value_destroy((char *) value);
  }

  return error_OK;
}

error critbit_show(const critbit_t      *t,
                   critbit_show_key     *key,
                   critbit_show_destroy *key_destroy,
                   critbit_show_value   *value,
                   critbit_show_destroy *value_destroy,
                   FILE                 *f)
{
  critbit__show_args_t args;

  args.key           = key;
  args.key_destroy   = key_destroy;
  args.value         = value;
  args.value_destroy = value_destroy;
  args.f             = f;

  return critbit__walk_internal((critbit_t *) t,
                                critbit_WALK_IN_ORDER | critbit_WALK_ALL,
                                critbit__node_show, &args);
}

