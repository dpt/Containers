/* int-kv.c -- functions for items which use ints */

#include <stdio.h>

#include "utils.h"

#include "int-kv.h"

size_t intkv_len(const void *key_)
{
  NOT_USED(key_);

  return sizeof(int);
}

int intkv_compare(const void *a_, const void *b_)
{
  int a = *((const int *) a_);
  int b = *((const int *) b_);

  if (a > b) return 1;
  else if (a < b) return -1;
  else return 0;
}

const char *intkv_fmt(const void *kv)
{
  static char str[32];

  sprintf(str, "%d", *((const int *) kv));

  return str;
}
