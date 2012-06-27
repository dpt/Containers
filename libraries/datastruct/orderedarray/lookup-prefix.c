/* --------------------------------------------------------------------------
 *    Name: lookup-prefix.c
 * Purpose: Associative array implemented as an ordered array
 * ----------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>

#include "base/errors.h"

#include "datastruct/orderedarray.h"

#include "impl.h"

typedef struct orderedarray_lookup_prefix_args
{
  const unsigned char         *uprefix;
  size_t                       prefixlen;
  orderedarray_found_callback *cb;
  void                        *opaque;
  int                          found;
}
orderedarray_lookup_prefix_args_t;

static error orderedarray__lookup_prefix(orderedarray__node_t *n,
                                         void                 *opaque)
{
  orderedarray_lookup_prefix_args_t *args = opaque;
  size_t                             prefixlen;

  prefixlen = args->prefixlen;

  if (n->item.keylen >= prefixlen &&
      memcmp(n->item.key, args->uprefix, prefixlen) == 0)
  {
    args->found = 1;
    return args->cb(&n->item, args->opaque);
  }
  else
  {
    return error_OK;
  }
}

error orderedarray_lookup_prefix(const orderedarray_t        *t,
                                 const void                  *prefix,
                                 size_t                       prefixlen,
                                 orderedarray_found_callback *cb,
                                 void                        *opaque)
{
  error                             err;
  orderedarray_lookup_prefix_args_t args;

  args.uprefix   = prefix;
  args.prefixlen = prefixlen;
  args.cb        = cb;
  args.opaque    = opaque;
  args.found     = 0;

  err = orderedarray__walk_internal((orderedarray_t *) t, // must cast away constness
                                    orderedarray__lookup_prefix,
                                    &args);
  if (err)
    return err;

  return args.found ? error_OK : error_NOT_FOUND;
}

