/* kv-common.c -- function(s) common to all kv callbacks */

#include <stdlib.h>

#include "memento.h"

#include "types.h"

#include "kv-common.h"

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
