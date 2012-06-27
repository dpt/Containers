/* --------------------------------------------------------------------------
 *    Name: destroy.c
 * Purpose: Queue implemented as a circular buffer
 * ----------------------------------------------------------------------- */

#include <stdlib.h>

#include "base/memento/memento.h"

#include "datastruct/queue.h"

void queue_destroy(queue_t *doomed)
{
  free(doomed);
}

