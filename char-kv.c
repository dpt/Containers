/* char-kv.c -- functions for items which use chars */

#include <ctype.h>
#include <stdio.h>

#include "utils.h"

#include "char-kv.h"

size_t charkv_len(const void *key_)
{
  NOT_USED(key_);

  return sizeof(char);
}

int charkv_compare(const void *a_, const void *b_)
{
  int a = *((const char *) a_);
  int b = *((const char *) b_);

  if (a > b) return 1;
  else if (a < b) return -1;
  else return 0;
}

const char *charkv_fmt(const void *kv)
{
  static char str[12];
  char        c;

  c = *((const char *) kv);

  if (isprint(c))
    sprintf(str, "%c", c);
  else
    sprintf(str, "[%d]", c);

  return str;
}
