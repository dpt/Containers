/* critbit-walk.c -- associative array implemented as critbit tree */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memento.h"

#include "types.h"
#include "string.h"
#include "string-kv.h"

#include "critbit.h"

#include "critbit-impl.h"

error critbit_walk(const critbit_t       *t,
                   critbit_walk_flags     flags,
                   critbit_walk_callback *cb,
                   void                  *opaque)
{
  return error_OK; // NYI
}
