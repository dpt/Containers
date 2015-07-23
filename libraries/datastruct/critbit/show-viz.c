/* --------------------------------------------------------------------------
 *    Name: show-viz.c
 * Purpose: Associative array implemented as a critbit tree
 * ----------------------------------------------------------------------- */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "base/result.h"
#include "base/types.h"

#include "datastruct/bstree.h"

#include "impl.h"

typedef struct critbit__show_viz_args
{
  critbit_show_key     *key;
  critbit_show_destroy *key_destroy;
  critbit_show_value   *value;
  critbit_show_destroy *value_destroy;
  FILE                 *f;

  int                   rank;
}
critbit__show_viz_args_t;

static int unsetbit(unsigned char otherbits)
{
  int i;

  for (i = 0; i < 8; i++)
    if ((otherbits & (128u >> i)) == 0) /* reversed sense */
      return i;

  assert(0);

  return -1;
}

static int bitdepth(const critbit__node_t *n)
{
  return n->byte * 8 + unsetbit(n->otherbits);
}

/* rank the nodes */
static result_t critbit__node_show_viz_rank(critbit__node_t *n, int level, void *opaque)
{
  critbit__show_viz_args_t *args = opaque;

  NOT_USED(level);

  if (bitdepth(n) == args->rank)
    (void) fprintf(args->f, "\t\t\"%p\"\n", (void *) n);

  return result_OK;
}

/* rank the leaf nodes */
static result_t critbit__node_show_viz_rank_leaves(critbit__node_t *n, int level, void *opaque)
{
  critbit__show_viz_args_t *args = opaque;

  NOT_USED(level);

  (void) fprintf(args->f, "\t\t\"%p\"\n", (void *) n);

  return result_OK;
}

/* link the nodes */
static result_t critbit__node_show_viz_link(critbit__node_t *n, int level, void *opaque)
{
  critbit__show_viz_args_t *args = opaque;

  NOT_USED(level);

  if (IS_EXTERNAL(n))
  {
    critbit__extnode_t *m;
    const char         *key;
    const char         *value;

    m = FROM_STORE(n);

    key   = args->key   && m->item.key   ? args->key(m->item.key)     : NULL;
    value = args->value && m->item.value ? args->value(m->item.value) : NULL;

    (void) fprintf(args->f, "\t\"%p\" [shape=record, label=\"{%s|%s}\"];\n",
                   (void *) n,
                   key   ? key   : "(null)",
                   value ? value : "(null)");

    if (args->key_destroy   && key)   args->key_destroy((char *) key);
    if (args->value_destroy && value) args->value_destroy((char *) value);
  }
  else
  {
    (void) fprintf(args->f, "\t\"%p\" [shape=record, label=\"{bit %d}\"];\n",
                   (void *) n,
                   bitdepth(n));
    if (n->child[0]) // are these tests redundant?
      (void) fprintf(args->f, "\t\"%p\":sw -> \"%p\":n;\n", n, n->child[0]);
    if (n->child[1])
      (void) fprintf(args->f, "\t\"%p\":se -> \"%p\":n;\n", n, n->child[1]);
  }

  return result_OK;
}

/* ----------------------------------------------------------------------- */

static void critbit__max_rank_r(const critbit__node_t *n, int *depth)
{
  if (IS_EXTERNAL(n))
  {
  }
  else
  {
    int d;
    int i;

    d = bitdepth(n);

    if (d > *depth)
      *depth = d;

    for (i = 0; i < 2; i++)
      critbit__max_rank_r(n->child[i], depth);
  }
}

static int critbit__max_rank(const critbit_t *t)
{
  int depth;

  depth = -1;

  critbit__max_rank_r(t->root, &depth);

  return depth;
}

/* ----------------------------------------------------------------------- */

static int critbit__count_for_rank_r(const critbit__node_t *n,
                                     int                    rank,
                                     int                    count)
{
  if (IS_EXTERNAL(n))
  {
    return count;
  }
  else
  {
    int i;

    for (i = 0; i < 2; i++)
      count = critbit__count_for_rank_r(n->child[i], rank, count);

    return count + (bitdepth(n) == rank);
  }
}

static int critbit__count_for_rank(const critbit_t *t, int rank)
{
  return critbit__count_for_rank_r(t->root, rank, 0);
}

/* ----------------------------------------------------------------------- */

result_t critbit_show_viz(const critbit_t      *t,
                          critbit_show_key     *key,
                          critbit_show_destroy *key_destroy,
                          critbit_show_value   *value,
                          critbit_show_destroy *value_destroy,
                          FILE                 *f)
{
  result_t                 err;
  critbit__show_viz_args_t args;
  int                      maxrank;
  int                      i;
  int                      counts[200]; // up to 200 levels (common prefix test hits)

  args.key           = key;
  args.key_destroy   = key_destroy;
  args.value         = value;
  args.value_destroy = value_destroy;
  args.f             = f;

  (void) fprintf(f, "digraph \"critbit\"\n");
  (void) fprintf(f, "{\n");
  (void) fprintf(f, "\tnode [shape = circle];\n");

  /* breadthwalk doesn't make sense for critbit trie - we need to rank the nodes by their critical bit */

  maxrank = critbit__max_rank(t);
  (void) fprintf(f, "\t/* maxrank=%d */\n", maxrank);

  for (i = 0; i <= maxrank; i++) /* note <= */
  {
    counts[i] = critbit__count_for_rank(t, i);
    (void) fprintf(f, "\t/* rank %d has %d nodes */\n", i, counts[i]);
  }

  /* draw a depth chart on the left hand side to force nodes to line up*/

  (void) fprintf(f, "{\n");
  (void) fprintf(f, "\tnode [shape=plaintext, fontsize=16];\n");
  fprintf(f, "\t-1"); /* we always have a node at this level */ // fix?
  for (i = 0; i <= maxrank; i++)
    if (counts[i])
      fprintf(f, " -> %d", i);
  (void) fprintf(f, " -> leaves;\n");
  (void) fprintf(f, "\t}\n");

  /* group nodes of the same critbit */

  for (i = -1; i <= maxrank; i++)
  {
    /* there's always a node at level -1 */ // fix?
    if (i >= 0 && counts[i] == 0)
      continue;

    (void) fprintf(f, "\t{ rank=same; %d /* level %d */\n", i, i);

    args.rank = i;

    err = critbit__walk_internal((critbit_t *) t,
                                 critbit_WALK_IN_ORDER | critbit_WALK_BRANCHES,
                                 critbit__node_show_viz_rank,
                                 &args);
    if (err)
      return err;

    (void) fprintf(f, "\t}\n");
  }

  /* group leaf nodes */
  {
    (void) fprintf(f, "\t{ rank=same; leaves;\n");

    err = critbit__walk_internal((critbit_t *) t,
                                 critbit_WALK_IN_ORDER | critbit_WALK_LEAVES,
                                 critbit__node_show_viz_rank_leaves,
                                 &args);
    if (err)
      return err;

    (void) fprintf(f, "\t}\n");
  }

  err = critbit__walk_internal((critbit_t *) t,
                               critbit_WALK_IN_ORDER | critbit_WALK_BRANCHES | critbit_WALK_LEAVES,
                               critbit__node_show_viz_link, &args);
  if (err)
    return err;

  (void) fprintf(f, "}\n");

  return result_OK;
}

