/* --------------------------------------------------------------------------
 *    Name: full.c
 * Purpose: Queue
 * ----------------------------------------------------------------------- */

#include <stddef.h>

#include "datastruct/queue.h"

#include "impl.h"

int queue_full(const queue_t *q)
{
  ptrdiff_t head, tail;

  head = q->head - q->buffer;
  tail = q->tail - q->buffer;

  return ((head + q->width) % (q->nelems * q->width)) == tail;
}

