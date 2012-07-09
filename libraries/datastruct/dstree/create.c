/* --------------------------------------------------------------------------
 *    Name: create.c
 * Purpose: Associative array implemented as a digital search tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <stdlib.h>

#include "base/memento/memento.h"
#include "base/errors.h"

#include "datastruct/critbit.h"

#include "impl.h"

error dstree_create(const void            *default_value,
                    dstree_destroy_key    *destroy_key,
                    dstree_destroy_value  *destroy_value,
                    dstree_t             **pt)
{
  dstree_t *t;

  *pt = NULL;

  t = malloc(sizeof(*t));
  if (t == NULL)
    return error_OOM;

  t->root          = NULL;
  t->default_value = default_value;
  t->destroy_key   = destroy_key;
  t->destroy_value = destroy_value;

  t->count         = 0;

  *pt = t;

  return error_OK;
}
