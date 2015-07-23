/* --------------------------------------------------------------------------
 *    Name: create.c
 * Purpose: Associative array implemented as a linked list
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "base/result.h"

#include "datastruct/linkedlist.h"

#include "impl.h"

result_t linkedlist_create(const void                *default_value,
                           linkedlist_compare        *compare,
                           linkedlist_destroy_key    *destroy_key,
                           linkedlist_destroy_value  *destroy_value,
                           linkedlist_t             **pt)
{
  linkedlist_t *t;

  *pt = NULL;

  t = malloc(sizeof(*t));
  if (t == NULL)
    return result_OOM;

  t->anchor        = NULL;

  t->default_value = default_value;
  t->compare       = compare;
  t->destroy_key   = destroy_key;
  t->destroy_value = destroy_value;

  t->count         = 0;

  *pt = t;

  return result_OK;
}

