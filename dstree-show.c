/* dstree-show.c -- associative array implemented as digital search tree */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memento.h"

#include "types.h"
#include "string.h"
#include "string-kv.h"

#include "dstree.h"

#include "dstree-impl.h"

/* ----------------------------------------------------------------------- */

typedef struct dstree__show_args
{
  dstree_show_key     *key;
  dstree_show_destroy *key_destroy;
  dstree_show_value   *value;
  dstree_show_destroy *value_destroy;
  FILE                *f;
}
dstree__show_args_t;

static error dstree__node_show(dstree__node_t *n, int level, void *opaque)
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

  return error_OK;
}

error dstree_show(const dstree_t      *t,
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

  // wah wah wah - this will fail as stringkv_fmt has a single internal buffer

  if (args.key == NULL)
    args.key = stringkv_fmt;
  if (args.value == NULL)
    args.value = stringkv_fmt;
  if (args.key_destroy == NULL)
    args.key_destroy = stringkv_fmt_destroy;
  if (args.value_destroy == NULL)
    args.value_destroy = stringkv_fmt_destroy;

  // this walks the tree in post order but we don't mind (as the tree is unordered)
  return dstree__walk_internal((dstree_t *) t, dstree__node_show, &args);
}

/* ----------------------------------------------------------------------- */
