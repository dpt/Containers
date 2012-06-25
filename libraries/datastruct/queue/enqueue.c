/* --------------------------------------------------------------------------
 *    Name: enqueue.c
 * Purpose: Queue
 * ----------------------------------------------------------------------- */

#include <string.h>

#include "base/errors.h"

#include "datastruct/queue.h"

#include "impl.h"

error queue_enqueue(queue_t *q, const void *value)
{
  if (queue_full(q))
    return error_QUEUE_FULL;

  memcpy(q->head, value, q->width);

  if (q->head + q->width == q->buffer + q->nelems * q->width)
    q->head = q->buffer;
  else
    q->head += q->width;

  return error_OK;
}

