/* --------------------------------------------------------------------------
 *    Name: lookup-prefix.c
 * Purpose: Associative array implemented as a binary search tree
 * ----------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>

#include "base/result.h"
#include "base/types.h"

#include "datastruct/bstree.h"

#include "impl.h"

typedef struct bstree_lookup_prefix_args
{
  const unsigned char   *uprefix;
  size_t                 prefixlen;
  bstree_found_callback *cb;
  void                  *opaque;
  int                    found;
}
bstree_lookup_prefix_args_t;

static result_t bstree__lookup_prefix(bstree__node_t *n,
                                      int             level,
                                      void           *opaque)
{
  bstree_lookup_prefix_args_t *args = opaque;
  size_t                       len;

  NOT_USED(level);

  len = args->prefixlen;

  if (n->item.keylen >= len && memcmp(n->item.key, args->uprefix, len) == 0)
  {
    args->found = 1;
    return args->cb(&n->item, args->opaque);
  }

  return result_OK;
}

/* Walk the entire tree looking for matches.
 * Is there a more cunning way than this? */
result_t bstree_lookup_prefix(const bstree_t        *t,
                              const void            *prefix,
                              size_t                 prefixlen,
                              bstree_found_callback *cb,
                              void                  *opaque)
{
  result_t                    err;
  bstree_lookup_prefix_args_t args;

  args.uprefix   = prefix;
  args.prefixlen = prefixlen;
  args.cb        = cb;
  args.opaque    = opaque;
  args.found     = 0;

  err = bstree__walk_internal((bstree_t *) t, /* must cast away constness */
                              bstree__lookup_prefix,
                              &args);
  if (err)
    return err;

  return args.found ? result_OK : result_NOT_FOUND;
}

