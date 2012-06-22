/* item.h -- basic (key,value) pair structure */

#ifndef ITEM_H
#define ITEM_H

#include <stdlib.h>

typedef struct item
{
  /* we declare key and value const and cast it away as necessary */
  const void *key;
  size_t      keylen;
  const void *value;
}
item_t;

#endif /* ITEM_H */
