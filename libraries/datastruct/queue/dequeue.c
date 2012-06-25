/* --------------------------------------------------------------------------
 *    Name: dequeue.c
 * Purpose: Queue
 * ----------------------------------------------------------------------- */

#include <string.h>

#include "base/errors.h"

#include "datastruct/queue.h"

#include "impl.h"

error queue_dequeue(queue_t *q, void *value)
{
  if (queue_empty(q))
    return error_QUEUE_EMPTY;

  memcpy(value, q->tail, q->width);

  if (q->tail + q->width == q->buffer + q->nelems * q->width)
    q->tail = q->buffer;
  else
    q->tail += q->width;

  return error_OK;
}

