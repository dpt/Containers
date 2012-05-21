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

  // this walks the tree in post order but we don't mind (as the tree is unordered)
  return dstree__walk_internal((dstree_t *) t, dstree__node_show, &args);
}

/* ----------------------------------------------------------------------- */

typedef struct dstree__show_viz_args
{
  dstree_show_key     *key;
  dstree_show_destroy *key_destroy;
  dstree_show_value   *value;
  dstree_show_destroy *value_destroy;
  FILE                 *f;

  int                   lastlevel;
}
dstree__show_viz_args_t;

/* rank the nodes */
static error dstree__node_show_viz_rank(dstree__node_t *n, int level, void *opaque)
{
  dstree__show_viz_args_t *args = opaque;

  if (level > args->lastlevel)
  {
    if (args->lastlevel >= 0)
      (void) fprintf(args->f, "\t}\n");

    (void) fprintf(args->f, "\t{ rank=same; /* level %d */\n", level);

    args->lastlevel = level;
  }

  (void) fprintf(args->f, "\t\t\"%p\"\n", n);

  return error_OK;
}

/* link the nodes */
static error dstree__node_show_viz_link(dstree__node_t *n, int level, void *opaque)
{
  dstree__show_viz_args_t *args = opaque;
  const char              *key;
  const char              *value;

  if (level > args->lastlevel)
  {
    if (args->lastlevel >= 0)
      (void) fprintf(args->f, "\t}\n");

    (void) fprintf(args->f, "\tsubgraph\n");
    (void) fprintf(args->f, "\t{\n");

    args->lastlevel = level;
  }

  key   = args->key   && n->item.key   ? args->key(n->item.key)     : NULL;
  value = args->value && n->item.value ? args->value(n->item.value) : NULL;

  (void) fprintf(args->f, "\t\"%p\" [shape=record, label=\"%s|%s\"];\n", n, key, value);
  if (n->child[0] != NULL)
    (void) fprintf(args->f, "\t\"%p\":sw -> \"%p\":n;\n", n, n->child[0]);
  if (n->child[1] != NULL)
    (void) fprintf(args->f, "\t\"%p\":se -> \"%p\":n;\n", n, n->child[1]);

  if (args->key_destroy   && key)   args->key_destroy((char *) key);
  if (args->value_destroy && value) args->value_destroy((char *) value);

  return error_OK;
}

error dstree_show_viz(const dstree_t      *t,
                      dstree_show_key     *key,
                      dstree_show_destroy *key_destroy,
                      dstree_show_value   *value,
                      dstree_show_destroy *value_destroy,
                      FILE                *f)
{
  error                    err;
  dstree__show_viz_args_t args;

  args.key           = key;
  args.key_destroy   = key_destroy;
  args.value         = value;
  args.value_destroy = value_destroy;
  args.f             = f;

  args.lastlevel = -1;

  (void) fprintf(f, "digraph \"dst\"\n");
  (void) fprintf(f, "{\n");
  (void) fprintf(f, "\tnode [shape = circle];\n");

  err = dstree__breadthwalk_internal((dstree_t *) t,
                                     dstree__node_show_viz_rank, &args);
  if (err)
    return err;

  (void) fprintf(f, "\t}\n");

  err = dstree__walk_internal((dstree_t *) t,
                              dstree__node_show_viz_link, &args);
  if (err)
    return err;


  (void) fprintf(f, "}\n");

  return error_OK;
}
