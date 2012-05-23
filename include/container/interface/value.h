/* value.h -- interface of values */

#ifndef ICONTAINER_VALUE_H
#define ICONTAINER_VALUE_H

#include "container/interface/kv.h"

/* Is it correct to term this an interface? It contains a single data member
 * which would seem to invalidate that. If default_value were a function we
 * could sidestep this issue. */

typedef struct icontainer_value
{
  const void     *default_value;
  icontainer_kv_t kv;
}
icontainer_value_t;

#endif /* ICONTAINER_VALUE_H */
