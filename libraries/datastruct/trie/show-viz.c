/* --------------------------------------------------------------------------
 *    Name: show-viz.c
 * Purpose: Associative array implemented as a trie
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "base/result.h"

#include "datastruct/trie.h"

#include "impl.h"

typedef struct trie__show_viz_args
{
  trie_show_key     *key;
  trie_show_destroy *key_destroy;
  trie_show_value   *value;
  trie_show_destroy *value_destroy;
  FILE              *f;

  int                lastlevel;
}
trie__show_viz_args_t;

/* rank the nodes */
static result_t trie__node_show_viz_rank(trie__node_t *n,
                                         int           level,
                                         void         *opaque)
{
  trie__show_viz_args_t *args = opaque;

  if (level > args->lastlevel)
  {
    if (args->lastlevel >= 0)
      (void) fprintf(args->f, "\t}\n");

    (void) fprintf(args->f, "\t{ rank=same; /* level %d */\n", level);

    args->lastlevel = level;
  }

  (void) fprintf(args->f, "\t\t\"%p\"\n", n);

  return result_OK;
}

/* link the nodes */
static result_t trie__node_show_viz_link(trie__node_t *n,
                                         int           level,
                                         void         *opaque)
{
  trie__show_viz_args_t *args = opaque;
  const char            *key;
  const char            *value;

  key   = args->key   && n->item.key   ? args->key(n->item.key)     : NULL;
  value = args->value && n->item.value ? args->value(n->item.value) : NULL;

  if (level > args->lastlevel)
  {
    if (args->lastlevel >= 0)
      (void) fprintf(args->f, "\t}\n");

    (void) fprintf(args->f, "\tsubgraph\n");
    (void) fprintf(args->f, "\t{\n");

    args->lastlevel = level;
  }

  if (IS_LEAF(n))
  {
    (void) fprintf(args->f, "\t\"%p\" [shape=record, label=\"%s|%s\"];\n", n, key, value);
  }
  else
  {
    if (n->child[0] != NULL)
      (void) fprintf(args->f, "\t\"%p\":sw -> \"%p\":n;\n", n, n->child[0]);
    if (n->child[1] != NULL)
      (void) fprintf(args->f, "\t\"%p\":se -> \"%p\":n;\n", n, n->child[1]);
  }

  if (args->key_destroy   && key)   args->key_destroy((char *) key);
  if (args->value_destroy && value) args->value_destroy((char *) value);

  return result_OK;
}

result_t trie_show_viz(const trie_t      *t,
                       trie_show_key     *key,
                       trie_show_destroy *key_destroy,
                       trie_show_value   *value,
                       trie_show_destroy *value_destroy,
                       FILE              *f)
{
  result_t              err;
  trie__show_viz_args_t args;

  args.key           = key;
  args.key_destroy   = key_destroy;
  args.value         = value;
  args.value_destroy = value_destroy;
  args.f             = f;

  args.lastlevel = -1;

  (void) fprintf(f, "digraph \"trie\"\n");
  (void) fprintf(f, "{\n");
  (void) fprintf(f, "\tnode [shape = circle];\n");

  err = trie__breadthwalk_internal((trie_t *) t,
                                   trie_WALK_IN_ORDER | trie_WALK_BRANCHES | trie_WALK_LEAVES,
                                   trie__node_show_viz_rank, &args);
  if (err)
    return err;

  (void) fprintf(f, "\t}\n");

  err = trie__walk_internal((trie_t *) t,
                            trie_WALK_IN_ORDER | trie_WALK_BRANCHES | trie_WALK_LEAVES,
                            trie__node_show_viz_link, &args);
  if (err)
    return err;

  (void) fprintf(f, "}\n");

  return result_OK;
}
