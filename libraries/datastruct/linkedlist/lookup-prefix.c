/* --------------------------------------------------------------------------
 *    Name: lookup-prefix.c
 * Purpose: Linked list
 * ----------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>

#include "base/errors.h"

#include "datastruct/linkedlist.h"

#include "impl.h"

typedef struct linkedlist_lookup_prefix_args
{
  const unsigned char       *uprefix;
  size_t                     prefixlen;
  linkedlist_found_callback *cb;
  void                      *opaque;
  int                        found;
}
linkedlist_lookup_prefix_args_t;

static error linkedlist__lookup_prefix(linkedlist__node_t *n,
                                       void               *opaque)
{
  linkedlist_lookup_prefix_args_t *args = opaque;
  size_t                           prefixlen;

  prefixlen = args->prefixlen;

  if (n->item.keylen >= prefixlen &&
      memcmp(n->item.key, args->uprefix, prefixlen) == 0)
  {
    args->found = 1;
    return args->cb(&n->item, args->opaque);
  }

  return error_OK;
}

error linkedlist_lookup_prefix(const linkedlist_t        *t,
                               const void                *prefix,
                               size_t                     prefixlen,
                               linkedlist_found_callback *cb,
                               void                      *opaque)
{
  error                           err;
  linkedlist_lookup_prefix_args_t args;

  args.uprefix   = prefix;
  args.prefixlen = prefixlen;
  args.cb        = cb;
  args.opaque    = opaque;

  err = linkedlist__walk_internal((linkedlist_t *) t, // must cast away constness
                                  linkedlist__lookup_prefix,
                                  &args);
  if (err)
    return err;

  return args.found ? error_OK : error_NOT_FOUND;
}

