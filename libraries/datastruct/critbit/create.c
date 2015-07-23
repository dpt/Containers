/* --------------------------------------------------------------------------
 *    Name: create.c
 * Purpose: Associative array implemented as a critbit tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <stdlib.h>

#include "base/memento/memento.h"
#include "base/result.h"

#include "datastruct/critbit.h"

#include "impl.h"

result_t critbit_create(const void             *default_value,
                        critbit_destroy_key    *destroy_key,
                        critbit_destroy_value  *destroy_value,
                        critbit_t             **pt)
{
  critbit_t *t;

  *pt = NULL;

  t = malloc(sizeof(*t));
  if (t == NULL)
    return result_OOM;

  t->root          = NULL;
  t->default_value = default_value;
  t->destroy_key   = destroy_key;
  t->destroy_value = destroy_value;

  t->intcount      = 0;
  t->extcount      = 0;

  *pt = t;

  return result_OK;
}

