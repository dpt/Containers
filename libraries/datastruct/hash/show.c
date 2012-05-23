/* --------------------------------------------------------------------------
 *    Name: show.c
 * Purpose: Hash
 * ----------------------------------------------------------------------- */

#include "datastruct/hash.h"

#include "impl.h"

typedef struct hash__show_args
{
  hash_show_key     *key;
  hash_show_destroy *key_destroy;
  hash_show_value   *value;
  hash_show_destroy *value_destroy;
  FILE              *f;
}
hash__show_args_t;

static error hash__node_show(hash__node_t *n,
                             int           bin,
                             int           index,
                             void         *opaque)
{
  hash__show_args_t *args = opaque;
  const char        *key;
  const char        *value;

  key   = args->key   && n->item.key   ? args->key(n->item.key)     : NULL;
  value = args->value && n->item.value ? args->value(n->item.value) : NULL;

  (void) fprintf(args->f, "hash: %p: %d[%d]: %s -> %s\n",
                 n, bin, index,
                 key   ? key   : "(null)",
                 value ? value : "(null)");

  if (args->key_destroy   && key)   args->key_destroy((char *) key);
  if (args->value_destroy && value) args->value_destroy((char *) value);

  return error_OK;
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

  return hash__walk_internal(t, hash__node_show, &args);
}
