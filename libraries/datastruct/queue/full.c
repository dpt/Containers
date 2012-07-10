/* --------------------------------------------------------------------------
 *    Name: full.c
 * Purpose: Queue implemented as a circular buffer
 * ----------------------------------------------------------------------- */

#include <stddef.h>

#include "datastruct/queue.h"

#include "impl.h"

int queue_full(const queue_t *q)
{
  ptrdiff_t head, tail;

  head = q->head - q->buffer;
  tail = q->tail - q->buffer;

  return (ptrdiff_t) ((head + q->width) % (q->nelems * q->width)) == tail;
}
