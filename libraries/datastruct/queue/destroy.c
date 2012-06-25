/* --------------------------------------------------------------------------
 *    Name: destroy.c
 * Purpose: Queue
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "datastruct/queue.h"

void queue_destroy(queue_t *doomed)
{
  free(doomed);
}

