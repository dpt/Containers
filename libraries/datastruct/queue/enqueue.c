/* --------------------------------------------------------------------------
 *    Name: enqueue.c
 * Purpose: Queue implemented as a circular buffer
 * ----------------------------------------------------------------------- */

#include <string.h>

#include "base/result.h"

#include "datastruct/queue.h"

#include "impl.h"

result_t queue_enqueue(queue_t *q, const void *value)
{
  if (queue_full(q))
    return result_QUEUE_FULL;

  memcpy(q->head, value, q->width);

  if (q->head + q->width == q->buffer + q->nelems * q->width)
    q->head = q->buffer;
  else
    q->head += q->width;

  return result_OK;
}

