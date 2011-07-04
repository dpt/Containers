/* item.h -- basic (key,value) pair structure */

#ifndef ITEM_H
#define ITEM_H

typedef struct item
{
  /* we declare key and value const and cast it away as necessary */
  const void *key, *value;
}
item_t;

#endif /* ITEM_H */
