/* --------------------------------------------------------------------------
 *    Name: show-viz.c
 * Purpose: Binary search tree
 * ----------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>

#include "base/result.h"
#include "base/types.h"
#include "base/utils.h"

#include "datastruct/bstree.h"

#include "impl.h"

typedef struct bstree__show_viz_args
{
  bstree_show_key     *key;
  bstree_show_destroy *key_destroy;
  bstree_show_value   *value;
  bstree_show_destroy *value_destroy;
  FILE                *f;
}
bstree__show_viz_args_t;

static result_t bstree__node_show_viz(bstree__node_t *n,
                                      int             level,
                                      void           *opaque)
{
  bstree__show_viz_args_t *args = opaque;
  const char              *key;
  const char              *value;

  NOT_USED(level);

  key   = args->key   && n->item.key   ? args->key(n->item.key)     : NULL;
  value = args->value && n->item.value ? args->value(n->item.value) : NULL;

  (void) fprintf(args->f, "\t\"%p\" [shape=record, label=\"%s|%s\"];\n",
                 n,
                 key   ? key   : "(null)",
                 value ? value : "(null)");
  if (n->child[0])
    (void) fprintf(args->f, "\t\"%p\":sw -> \"%p\":n;\n", n, n->child[0]);
  if (n->child[1])
    (void) fprintf(args->f, "\t\"%p\":se -> \"%p\":n;\n", n, n->child[1]);

  if (args->key_destroy   && key)   args->key_destroy((char *) key);
  if (args->value_destroy && value) args->value_destroy((char *) value);

  return result_OK;
}

result_t bstree_show_viz(const bstree_t      *t,
                         bstree_show_key     *key,
                         bstree_show_destroy *key_destroy,
                         bstree_show_value   *value,
                        bstree_show_destroy *value_destroy,
                        FILE                *f)
{
  result_t                err;
  bstree__show_viz_args_t args;

  args.key           = key;
  args.key_destroy   = key_destroy;
  args.value         = value;
  args.value_destroy = value_destroy;
  args.f             = f;

  (void) fprintf(f, "digraph \"bstree\"\n");
  (void) fprintf(f, "{\n");
  //(void) fprintf(f, "\tnode [shape = circle];\n");

  err = bstree__walk_internal((bstree_t *) t, bstree__node_show_viz, &args);
  if (err)
    return err;

  (void) fprintf(f, "}\n");

  return result_OK;
}
