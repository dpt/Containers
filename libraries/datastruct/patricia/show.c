/* --------------------------------------------------------------------------
 *    Name: show.c
 * Purpose: Associative array implemented as a PATRICIA tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "base/errors.h"

#include "datastruct/patricia.h"

#include "impl.h"

typedef struct patricia__show_args
{
  patricia_show_key     *key;
  patricia_show_destroy *key_destroy;
  patricia_show_value   *value;
  patricia_show_destroy *value_destroy;
  FILE                  *f;
}
patricia__show_args_t;

static error patricia__node_show(patricia__node_t *n, int level, void *opaque)
{
  static const char stars[] = "********************************"; /* only works up to 32 levels deep */

  patricia__show_args_t *args = opaque;
  const char            *key;
  const char            *value;

  key   = args->key   && n->item.key   ? args->key(n->item.key)     : NULL;
  value = args->value && n->item.value ? args->value(n->item.value) : NULL;

  (void) fprintf(args->f, "patricia: %p: %-32.*s : %s -> %s\n",
                 (void *) n, level + 1, stars,
                 key   ? key   : "(null)",
                 value ? value : "(null)");

  if (args->key_destroy   && key)   args->key_destroy((char *) key);
  if (args->value_destroy && value) args->value_destroy((char *) value);

  return error_OK;
}

error patricia_show(const patricia_t      *t,
                    patricia_show_key     *key,
                    patricia_show_destroy *key_destroy,
                    patricia_show_value   *value,
                    patricia_show_destroy *value_destroy,
                    FILE                  *f)
{
  patricia__show_args_t args;

  args.key           = key;
  args.key_destroy   = key_destroy;
  args.value         = value;
  args.value_destroy = value_destroy;
  args.f             = f;

  return patricia__walk_internal((patricia_t *) t,
                                 patricia_WALK_LEAVES,
                                 patricia__node_show, &args);
}

