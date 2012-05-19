/* linkedlist-select.c */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memento.h"

#include "types.h"
#include "string.h"
#include "string-kv.h"

#include "linkedlist.h"

#include "linkedlist-impl.h"

/* ----------------------------------------------------------------------- */

const item_t *linkedlist_select(linkedlist_t *t, int k)
{
  linkedlist__node_t *n;

  for (n = t->anchor; n; n = n->next)
    if (k-- == 0)
      break;

  return n ? &n->item : NULL;
}

/* ----------------------------------------------------------------------- */

