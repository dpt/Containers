/* common.c -- function(s) common to all kv callbacks */

#include <stdlib.h>

#include "base/memento/memento.h"
#include "base/types.h"

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
