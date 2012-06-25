/* --------------------------------------------------------------------------
 *    Name: create.c
 * Purpose: Associative array implemented as a binary search tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <stdlib.h>

#include "base/memento/memento.h"
#include "base/errors.h"

#include "datastruct/bstree.h"

#include "impl.h"

error bstree_create(const void            *default_value,
                    bstree_compare        *compare,
                    bstree_destroy_key    *destroy_key,
                    bstree_destroy_value  *destroy_value,
                    bstree_t             **pt)
{
  bstree_t *t;

  *pt = NULL;

  t = malloc(sizeof(*t));
  if (t == NULL)
    return error_OOM;

  t->root          = NULL;
  t->default_value = default_value;
  t->compare       = compare;
  t->destroy_key   = destroy_key;
  t->destroy_value = destroy_value;

  t->count         = 0;

  *pt = t;

  return error_OK;
}

