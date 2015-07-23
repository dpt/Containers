/* --------------------------------------------------------------------------
 *    Name: maker.h
 * Purpose: Interface of a container making function
 * ----------------------------------------------------------------------- */

#ifndef ICONTAINER_MAKER_H
#define ICONTAINER_MAKER_H

#include "base/result.h"

#include "container/interface/container.h"
#include "container/interface/key.h"
#include "container/interface/value.h"

typedef result_t (icontainer_maker)(icontainer_t            **container,
                                    const icontainer_key_t   *key,
                                    const icontainer_value_t *value);

#endif /* ICONTAINER_MAKER_H */

