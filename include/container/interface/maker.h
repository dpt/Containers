/* maker.h -- interface of a container making function */

#ifndef ICONTAINER_MAKER_H
#define ICONTAINER_MAKER_H

#include "base/errors.h"

#include "container/interface/container.h"
#include "container/interface/key.h"
#include "container/interface/value.h"

typedef error (icontainer_maker)(icontainer_t            **container,
                                 const icontainer_key_t   *key,
                                 const icontainer_value_t *value);

#endif /* ICONTAINER_MAKER_H */
