/* critbit-show.c -- associative array implemented as critbit tree */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memento.h"

#include "errors.h"
#include "types.h"
#include "string.h"
#include "string-kv.h"

#include "critbit.h"

#include "critbit-impl.h"

/* ----------------------------------------------------------------------- */

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
                   n, level + 1, stars, n->child[0], n->child[1]);
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

  // wah wah wah - this will fail as stringkv_fmt has a single internal buffer

  if (args.key == NULL)
    args.key = stringkv_fmt;
  if (args.value == NULL)
    args.value = stringkv_fmt;
  if (args.key_destroy == NULL)
    args.key_destroy = stringkv_fmt_destroy;
  if (args.value_destroy == NULL)
    args.value_destroy = stringkv_fmt_destroy;

  return critbit__walk_internal((critbit_t *) t,
                                critbit_WALK_IN_ORDER | critbit_WALK_ALL,
                                critbit__node_show, &args);
}

/* ----------------------------------------------------------------------- */

typedef struct critbit__show_viz_args
{
  critbit_show_key     *key;
  critbit_show_destroy *key_destroy;
  critbit_show_value   *value;
  critbit_show_destroy *value_destroy;
  FILE                 *f;

  int                   lastlevel;
}
critbit__show_viz_args_t;

/* rank the nodes */
static error critbit__node_show_viz_rank(critbit__node_t *n, int level, void *opaque)
{
  critbit__show_viz_args_t *args = opaque;

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
static error critbit__node_show_viz_link(critbit__node_t *n, int level, void *opaque)
{
  critbit__show_viz_args_t *args = opaque;

  if (level > args->lastlevel)
  {
    if (args->lastlevel >= 0)
      (void) fprintf(args->f, "\t}\n");

    (void) fprintf(args->f, "\tsubgraph\n");
    (void) fprintf(args->f, "\t{\n");

    args->lastlevel = level;
  }

  if (IS_EXTERNAL(n))
  {
    critbit__extnode_t *m;
    const char         *key;
    const char         *value;

    m = FROM_STORE(n);

    key   = args->key   && m->item.key   ? args->key(m->item.key)     : NULL;
    value = args->value && m->item.value ? args->value(m->item.value) : NULL;

    (void) fprintf(args->f, "\t\"%p\" [shape=record, label=\"%s|%s\"];\n", n, key, value);

    if (args->key_destroy   && key)   args->key_destroy((char *) key);
    if (args->value_destroy && value) args->value_destroy((char *) value);
  }
  else
  {
    char  binary[9];
    char *p;
    int   v;

    p = binary;
    v = n->otherbits;
    *p++ = '0' + ((v & 128) != 0);
    *p++ = '0' + ((v &  64) != 0);
    *p++ = '0' + ((v &  32) != 0);
    *p++ = '0' + ((v &  16) != 0);
    *p++ = '0' + ((v &   8) != 0);
    *p++ = '0' + ((v &   4) != 0);
    *p++ = '0' + ((v &   2) != 0);
    *p++ = '0' + ((v &   1) != 0);
    *p = '\0';

    (void) fprintf(args->f, "\t\"%p\" [shape=record, label=\"byte %d|bit %s\"];\n", n, n->byte, binary);
    if (n->child[0] != NULL) // are these tests redundant?
      (void) fprintf(args->f, "\t\"%p\":sw -> \"%p\":n;\n", n, n->child[0]);
    if (n->child[1] != NULL)
      (void) fprintf(args->f, "\t\"%p\":se -> \"%p\":n;\n", n, n->child[1]);
  }

  return error_OK;
}

error critbit_show_viz(const critbit_t      *t,
                       critbit_show_key     *key,
                       critbit_show_destroy *key_destroy,
                       critbit_show_value   *value,
                       critbit_show_destroy *value_destroy,
                       FILE                 *f)
{
  error                    err;
  critbit__show_viz_args_t args;

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

  args.lastlevel = -1;

  (void) fprintf(f, "digraph \"trie\"\n");
  (void) fprintf(f, "{\n");
  (void) fprintf(f, "\tnode [shape = circle];\n");

  err = critbit__breadthwalk_internal((critbit_t *) t,
                                      critbit_WALK_IN_ORDER | critbit_WALK_BRANCHES | critbit_WALK_LEAVES,
                                      critbit__node_show_viz_rank, &args);
  if (err)
    return err;

  (void) fprintf(f, "\t}\n");

  err = critbit__walk_internal((critbit_t *) t,
                               critbit_WALK_IN_ORDER | critbit_WALK_BRANCHES | critbit_WALK_LEAVES,
                               critbit__node_show_viz_link, &args);
  if (err)
    return err;


  (void) fprintf(f, "}\n");

  return error_OK;
}

