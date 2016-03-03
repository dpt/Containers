/* --------------------------------------------------------------------------
 *    Name: common.c
 * Purpose: Functions common across key and value types
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "base/types.h"
#include "base/utils.h"

#include "keyval/common.h"

void kv_nodestroy(void *doomed)
{
  NOT_USED(doomed);
}

void kv_fmtdestroy(char *doomed)
{
  free(doomed);
}

void kv_nofmtdestroy(char *doomed)
{
  NOT_USED(doomed);
}

