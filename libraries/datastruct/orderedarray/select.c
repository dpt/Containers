/* select.c */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base/memento/memento.h"

#include "base/types.h"
#include "string.h"
#include "keyval/string.h"

#include "datastruct/orderedarray.h"

#include "impl.h"

/* ----------------------------------------------------------------------- */

const item_t *orderedarray_select(orderedarray_t *t, int k)
{
  return (k < t->nelems) ? &t->array[k].item : t->default_value;
}

/* ----------------------------------------------------------------------- */

