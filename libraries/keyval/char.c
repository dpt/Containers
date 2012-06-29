/* --------------------------------------------------------------------------
 *    Name: char.c
 * Purpose: Functions for keys or values which are chars
 * ----------------------------------------------------------------------- */

#include <ctype.h>
#include <limits.h>
#include <stdio.h>

#include "base/types.h"

#include "keyval/char.h"

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

unsigned int charkv_hash(const void *key_)
{
  int c = *((const char *) key_); // does this need to be unsigned int?

  return c + CHAR_MIN;
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

