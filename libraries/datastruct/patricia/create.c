/* --------------------------------------------------------------------------
 *    Name: create.c
 * Purpose: Associative array implemented as a PATRICIA tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <stdlib.h>

#include "base/errors.h"

#include "datastruct/patricia.h"

#include "impl.h"

error patricia_create(const void              *default_value,
                      patricia_destroy_key    *destroy_key,
                      patricia_destroy_value  *destroy_value,
                      patricia_t             **pt)
{
  patricia_t *t;

  *pt = NULL;

  t = malloc(sizeof(*t));
  if (t == NULL)
    return error_OOM;

  /* the root node is only used for an all-zero-bits key */
  t->root = patricia__node_create(t, NULL, 0, NULL);
  if (t->root == NULL)
    return error_OOM;

  t->root->child[0] = t->root; /* left child points to self (for root node) */
  t->root->bit      = -1;

  t->default_value = default_value;
  t->destroy_key   = destroy_key;
  t->destroy_value = destroy_value;

  t->count         = 0;

  *pt = t;

  return error_OK;
}

