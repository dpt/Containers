/* --------------------------------------------------------------------------
 *    Name: create.c
 * Purpose: Associative array implemented as an ordered array
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "base/result.h"

#include "datastruct/orderedarray.h"

#include "impl.h"

result_t orderedarray_create(const void                  *default_value,
                             orderedarray_compare        *compare,
                             orderedarray_destroy_key    *destroy_key,
                             orderedarray_destroy_value  *destroy_value,
                             orderedarray_t             **pt)
{
  orderedarray_t *t;

  *pt = NULL;

  t = malloc(sizeof(*t));
  if (t == NULL)
    return result_OOM;

  t->array         = NULL;
  t->nelems        = 0;
  t->maxelems      = 0;

  t->default_value = default_value;
  t->compare       = compare;
  t->destroy_key   = destroy_key;
  t->destroy_value = destroy_value;

  *pt = t;

  return result_OK;
}

