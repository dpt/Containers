/* --------------------------------------------------------------------------
 *    Name: show.c
 * Purpose: Associative array implemented as a trie
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "base/errors.h"

#include "datastruct/trie.h"

#include "impl.h"

typedef struct trie__show_args
{
  trie_show_key     *key;
  trie_show_destroy *key_destroy;
  trie_show_value   *value;
  trie_show_destroy *value_destroy;
  FILE              *f;
}
trie__show_args_t;

static error trie__node_show(trie__node_t *n, int level, void *opaque)
{
  static const char stars[] = "********************************"; // works up to 32 levels deep

  trie__show_args_t *args = opaque;
  const char        *key;
  const char        *value;

  key   = args->key   && n->item.key   ? args->key(n->item.key)     : NULL;
  value = args->value && n->item.value ? args->value(n->item.value) : NULL;

  (void) fprintf(args->f, "trie: %p: %-32.*s : %s -> %s [l=%p r=%p]\n",
                 n, level + 1, stars,
                 key   ? key   : "(null)",
                 value ? value : "(null)",
                 n->child[0], n->child[1]);

  if (args->key_destroy   && key)   args->key_destroy((char *) key);
  if (args->value_destroy && value) args->value_destroy((char *) value);

  return error_OK;
}

error trie_show(const trie_t      *t,
                trie_show_key     *key,
                trie_show_destroy *key_destroy,
                trie_show_value   *value,
                trie_show_destroy *value_destroy,
                FILE              *f)
{
  trie__show_args_t args;

  args.key           = key;
  args.key_destroy   = key_destroy;
  args.value         = value;
  args.value_destroy = value_destroy;
  args.f             = f;

  return trie__walk_internal((trie_t *) t,
                             trie_WALK_IN_ORDER | trie_WALK_LEAVES,
                             trie__node_show, &args);
}
