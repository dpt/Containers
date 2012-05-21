/* --------------------------------------------------------------------------
 *    Name: show.c
 * Purpose: Hash
 * ----------------------------------------------------------------------- */

#include "hash.h"

#include "hash-impl.h"

typedef struct hash__show_args
{
  hash_show_key     *key;
  hash_show_destroy *key_destroy;
  hash_show_value   *value;
  hash_show_destroy *value_destroy;
  FILE              *f;
}
hash__show_args_t;

static int hash__node_show(const item_t *item, void *opaque)
{
  hash__show_args_t *args = opaque;
  const char        *key;
  const char        *value;

  key   = args->key   && item->key   ? args->key(item->key)     : NULL;
  value = args->value && item->value ? args->value(item->value) : NULL;

  (void) fprintf(args->f, "hash: %s -> %s\n",
                 key   ? key   : "(null)",
                 value ? value : "(null)");

  if (args->key_destroy   && key)   args->key_destroy((char *) key);
  if (args->value_destroy && value) args->value_destroy((char *) value);

  return 0;
}

error hash_show(const hash_t      *t,
                hash_show_key     *key,
                hash_show_destroy *key_destroy,
                hash_show_value   *value,
                hash_show_destroy *value_destroy,
                FILE              *f)
{
  hash__show_args_t args;

  args.key           = key;
  args.key_destroy   = key_destroy;
  args.value         = value;
  args.value_destroy = value_destroy;
  args.f             = f;

  return hash_walk((hash_t *) t, hash__node_show, &args);
}

