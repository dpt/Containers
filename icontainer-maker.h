/* icontainer-maker.h -- interface of a container making function */

#ifndef ICONTAINER_MAKER_H
#define ICONTAINER_MAKER_H

#include "errors.h"

#include "icontainer.h"
#include "icontainer-key.h"
#include "icontainer-value.h"

typedef error (icontainer_maker)(icontainer_t            **container,
                                 const icontainer_key_t   *key,
                                 const icontainer_value_t *value);

#endif /* ICONTAINER_MAKER_H */
