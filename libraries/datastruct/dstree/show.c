/* --------------------------------------------------------------------------
 *    Name: show.c
 * Purpose: Associative array implemented as a digital search tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "base/result.h"

#include "datastruct/dstree.h"

#include "impl.h"

typedef struct dstree__show_args
{
  dstree_show_key     *key;
  dstree_show_destroy *key_destroy;
  dstree_show_value   *value;
  dstree_show_destroy *value_destroy;
  FILE                *f;
}
dstree__show_args_t;

static result_t dstree__node_show(dstree__node_t *n, int level, void *opaque)
{
  static const char stars[] = "********************************"; // works up to 32 levels deep

  dstree__show_args_t *args = opaque;
  const char          *key;
  const char          *value;

  key   = args->key   && n->item.key   ? args->key(n->item.key)     : NULL;
  value = args->value && n->item.value ? args->value(n->item.value) : NULL;

  (void) fprintf(args->f, "dst: %p: %-32.*s : %s -> %s\n",
                 n, level + 1, stars,
                 key   ? key   : "(null)",
                 value ? value : "(null)");

  if (args->key_destroy   && key)   args->key_destroy((char *) key);
  if (args->value_destroy && value) args->value_destroy((char *) value);

  return result_OK;
}

result_t dstree_show(const dstree_t      *t,
                     dstree_show_key     *key,
                     dstree_show_destroy *key_destroy,
                     dstree_show_value   *value,
                     dstree_show_destroy *value_destroy,
                     FILE                *f)
{
  dstree__show_args_t args;

  args.key           = key;
  args.key_destroy   = key_destroy;
  args.value         = value;
  args.value_destroy = value_destroy;
  args.f             = f;

  /* this walks the tree in post order but we don't mind (as the tree is
   * unordered) */
  return dstree__walk_internal((dstree_t *) t, dstree__node_show, &args);
}

