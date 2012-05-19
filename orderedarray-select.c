/* orderedarray-select.c */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memento.h"

#include "types.h"
#include "string.h"
#include "string-kv.h"

#include "orderedarray.h"

#include "orderedarray-impl.h"

/* ----------------------------------------------------------------------- */

const item_t *orderedarray_select(orderedarray_t *t, int k)
{
  return (k < t->nelems) ? &t->array[k].item : t->default_value;
}

/* ----------------------------------------------------------------------- */

