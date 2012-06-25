/* --------------------------------------------------------------------------
 *    Name: node-create.c
 * Purpose: Associative array implemented as a critbit tree
 * ----------------------------------------------------------------------- */

#include <stddef.h>
#include <stdlib.h>

#include "base/memento/memento.h"

#include "datastruct/critbit.h"

#include "impl.h"

critbit__node_t *critbit__node_create(critbit_t *t,
                                      int        byte,
                                      uint8_t    otherbits)
{
  critbit__node_t *n;

  n = malloc(sizeof(*n));
  if (n == NULL)
    return NULL;

  n->child[0]  = NULL;
  n->child[1]  = NULL;
  n->byte      = byte;
  n->otherbits = otherbits;

  t->intcount++;

  return n;
}

