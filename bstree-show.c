/* bstree-show.c -- associative array implemented as binary search tree */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memento.h"

#include "types.h"
#include "string.h"
#include "string-kv.h"

#include "bstree.h"

#include "bstree-impl.h"

/* ----------------------------------------------------------------------- */

typedef struct bstree__show_args
{
  bstree_show_key     *key;
  bstree_show_destroy *key_destroy;
  bstree_show_value   *value;
  bstree_show_destroy *value_destroy;
  FILE                *f;
}
bstree__show_args_t;

static error bstree__node_show(bstree__node_t *n, int level, void *opaque)
{
  static const char stars[] = "********************************"; // works up to 32 levels deep

  bstree__show_args_t *args = opaque;
  const char          *key;
  const char          *value;

  key   = args->key   && n->item.key   ? args->key(n->item.key)     : NULL;
  value = args->value && n->item.value ? args->value(n->item.value) : NULL;

  (void) fprintf(args->f, "bst: %p: %-32.*s : %s -> %s\n",
                 n, level + 1, stars,
                 key   ? key   : "(null)",
                 value ? value : "(null)");

  if (args->key_destroy   && key)   args->key_destroy((char *) key);
  if (args->value_destroy && value) args->value_destroy((char *) value);

  return error_OK;
}

error bstree_show(const bstree_t      *t,
                  bstree_show_key     *key,
                  bstree_show_destroy *key_destroy,
                  bstree_show_value   *value,
                  bstree_show_destroy *value_destroy,
                  FILE                *f)
{
  bstree__show_args_t args;

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

  return bstree__walk_internal((bstree_t *) t, bstree__node_show, &args);
}

/* ----------------------------------------------------------------------- */
