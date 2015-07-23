/* --------------------------------------------------------------------------
 *    Name: dequeue.c
 * Purpose: Queue implemented as a circular buffer
 * ----------------------------------------------------------------------- */

#include <string.h>

#include "base/result.h"

#include "datastruct/queue.h"

#include "impl.h"

result_t queue_dequeue(queue_t *q, void *value)
{
  if (queue_empty(q))
    return result_QUEUE_EMPTY;

  memcpy(value, q->tail, q->width);

  if (q->tail + q->width == q->buffer + q->nelems * q->width)
    q->tail = q->buffer;
  else
    q->tail += q->width;

  return result_OK;
}

