/* string-kv.c -- functions for items which use strings */

#include "types.h"

#include <string.h>

#include "string-kv.h"

size_t stringkv_len(const void *key_)
{
  const char *key = key_;

  return strlen(key);
}

int stringkv_compare(const void *a_, const void *b_)
{
  const char *a = a_;
  const char *b = b_;

  if (a && b)
    return strcmp(a, b);

  /* sort NULL keys after non-NULL */
  if (!a && !b) return  0;
  else if (!a)  return -1;
  else          return  1;
}

const char *stringkv_fmt(const void *kv)
{
  return kv;
}
